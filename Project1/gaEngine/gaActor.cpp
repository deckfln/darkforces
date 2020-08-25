#include "gaActor.h"

#include <stdio.h>
#include <iostream>

#include "../config.h"
#include "../framework/fwCollision.h"

#include "../gaEngine/gaPlayer.h"
#include "../gaEngine/gaBoundingBoxes.h"
#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/gaEntity.h"

#include "../darkforces/dfLevel.h"

gaActor::gaActor(
	int mclass, 
	const std::string& name, 
	fwCylinder& cylinder,		// collision cylinder
	const glm::vec3& feet,			// position of the feet 
	float eyes, 
	float ankle) :
	gaEntity(mclass, name),
	m_cylinder(cylinder),
	m_ankle(ankle),
	m_eyes(eyes)
{
	physical(true);
	translate(feet);
	m_modelAABB = fwAABBox(cylinder);
	updateWorldAABB();
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix);
}

/**
 * return data from the bounding cylinder
 */
float gaActor::radius(void) 
{
	return m_cylinder.radius();
}

float gaActor::height(void)
{
	return m_cylinder.height();
}

/**
 * Walk following the front vector
 */
bool gaActor::moveTo(time_t delta, glm::vec3& velocity)
{
	if (velocity == glm::vec3(0)) {
		return false;	// no action, let the entity live its life
	}

	glm::vec3 direction = velocity * m_speed;

	delta = 33;	// TODO: remove the fixed time step

	// move forward (include a test to see if we can step down)
	// align the object to the direction
	//convert the direction vector to a quaternion
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right;
	glm::vec3 up;

	right = glm::normalize(glm::cross(_up, velocity));
	up = glm::cross(velocity, right);

	m_transforms.m_forward = velocity * m_cylinder.radius();
	m_transforms.m_downward = glm::vec3(0, -m_ankle, 0);
	m_transforms.m_position = position() + direction;
	m_transforms.m_quaternion = glm::quatLookAt(velocity, up);
	m_transforms.m_scale = get_scale();

	sendDelayedMessage(gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_FALL,
		&m_transforms);

	return false;
}

/**
 * Walk folowing the front vector
 */
bool gaActor::moveTo_old(time_t delta, glm::vec3& velocity)
{
	glm::vec3 direction = velocity * m_speed;

	delta = 33;	// TODO: remove the fixed time step

	glm::vec3 intersection;
	glm::vec3 target;
	std::list<gaCollisionPoint> collisions;

	if (m_animation_time != 0) {
		// manage physic driven trajectory
		m_animation_time += delta;
		glm::vec3 t3x3(m_animation_time * m_animation_time / 2, m_animation_time, 1);
		target = m_physic * t3x3;
		direction = target - m_cylinder.position();

		// std::cerr << "fwControlThirdPerson::update x=" << m_position_lvl.x << " y=" << m_position_lvl.y << " z=" << m_position_lvl.z << std::endl;
		// std::cerr << "fwControlThirdPerson::update TARGET x=" << target.x << " y=" << target.y << " z=" << target.z << std::endl;

#ifdef DEBUG
		gaDebugLog(LOW_DEBUG, "fwControlThirdPerson::update", "delta="+std::to_string(m_cylinder.position().y - target.y));
#endif

		if (m_level->checkEnvironement(m_cylinder, direction, intersection, collisions)) {
			// hit somewhere => stop physic engine
			for (auto& collision : collisions) {
				switch (collision.m_location) {
				case fwCollisionLocation::BOTTOM:
					// lock to the feet
					m_cylinder.position().y = collision.m_position.y;
					m_animation_time = 0;
					break;

				case fwCollisionLocation::TOP:
					// lock down from the head
					m_cylinder.position().y = collision.m_position.y - m_cylinder.height();
					// fall trough to start a drop down

				case fwCollisionLocation::FRONT:
					// so, hit a wall but not a ground, stop moving and drop dow
#ifdef DEBUG
					gaDebugLog(LOW_DEBUG, "fwControlThirdPerson::update", "hit wall, fall down");
#endif
					m_animation_time = 33;
					m_physic[0][0] = 0;			m_physic[1][0] = 0;		m_physic[2][0] = m_cylinder.position().x;
					m_physic[0][1] = c_gravity;	m_physic[1][1] = 0;		m_physic[2][1] = m_cylinder.position().y;
					m_physic[0][2] = 0;			m_physic[1][2] = 0;		m_physic[2][2] = m_cylinder.position().z;
				}
			}
		}
		else {
			// no move, continue with the physic engine
			translate(target);
			m_cylinder.position(target);
			m_worldBounding = fwAABBox(m_cylinder);
		}
	}
	else {
		// move forward (include a test to see if we can step down)
		direction.y = c_gravity;
		target = m_cylinder.position() + direction;

		// check collision with entities
		g_gaWorld.checkCollision(this, m_cylinder, direction, collisions);

		// check collision with the level
		m_level->checkEnvironement(m_cylinder, direction, intersection, collisions);

		// and take action
		if (collisions.size() != 0) {
			for (auto& collision : collisions) {
				gaEntity* entity = nullptr;

				switch (collision.m_location) {
				case fwCollisionLocation::COLLIDE:
					// 'who' do we checkCollision with
					if (entity != nullptr) {
						entity->collideWith(this);
					}
					break;

				case fwCollisionLocation::FRONT:
				case fwCollisionLocation::LEFT:
					// hit a full wall
					// break 2D movement but keep vertical movement
					target.x = m_cylinder.position().x;
					target.z = m_cylinder.position().z;
					break;

				case fwCollisionLocation::FRONT_TOP:
				case fwCollisionLocation::TOP:
					// hit the ceiling or a adjoint ceiling
					// don't move
					return false;

				case fwCollisionLocation::FRONT_BOTTOM:
					// if we cannot step over
					if (collision.m_position.y - target.y > m_ankle) {
						// break 2D movement but keep vertical movement
						target.x = m_cylinder.position().x;
						target.z = m_cylinder.position().z;
					}
					break;

				case fwCollisionLocation::BOTTOM:
					// if we cannot step over
					if (collision.m_position.y - target.y > m_ankle) {
						return false;	// cannot move over the step
					}

					// lock to the feet
					target.y = collision.m_position.y;
					break;

				}
			}
		}
		else {
			// no collision at all => nothing under the feet of the actor => freefall
			// engage the physic engine
			m_physic[0][0] = 0;			m_physic[1][0] = direction.x/20.0f;		m_physic[2][0] = target.x;
			m_physic[0][1] = c_gravity; m_physic[1][1] = direction.y;		m_physic[2][1] = target.y;
			m_physic[0][2] = 0;			m_physic[1][2] = direction.z/20.0f;		m_physic[2][2] = target.z;

			m_animation_time = delta;

#ifdef DEBUG
			std::string message = "freefall x = " + std::to_string(m_cylinder.position().x) + " y = " + std::to_string(m_cylinder.position().y) + " z = " + std::to_string(m_cylinder.position().y);
			gaDebugLog(LOW_DEBUG, "fwControlThirdPerson::updateCamera", message);
#endif
		}

		translate(target);
		m_cylinder.position(target);
		m_worldBounding = fwAABBox(m_cylinder);
	}

	return m_animation_time != 0;	// if the physic engine is engaged
}	

/**
 * trigger a jump
 */
void gaActor::jump(const glm::vec3& velocity)
{
	glm::vec3 direction = velocity * m_speed;
	direction.y = c_jump;

	m_physic[0][0] = 0;			m_physic[1][0] = direction.x/20.0f;		m_physic[2][0] = m_cylinder.position().x;
	m_physic[0][1] = c_gravity; m_physic[1][1] = direction.y;		m_physic[2][1] = m_cylinder.position().y;
	m_physic[0][2] = 0;			m_physic[1][2] = direction.z/20.0f;		m_physic[2][2] = m_cylinder.position().z;

	m_animation_time = 33;
}

/**
 * let an entity deal with a situation
 */
void gaActor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case gaMessage::WORLD_INSERT: {
		// init physic engine if needed

		// move forward (include a test to see if we can step down)
		// align the object to the direction
		//convert the direction vector to a quaternion
		glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right;
		glm::vec3 up;

		glm::vec3 direction = m_parent->direction();

		if (direction != glm::vec3(0)) {
			direction = glm::normalize(direction);
			right = glm::normalize(glm::cross(_up, direction));
			up = glm::cross(direction, right);
		}
		else {
			up = _up;
		}

		m_transforms.m_forward = glm::vec3(0);
		m_transforms.m_downward = glm::vec3(0, -m_cylinder.height()/2.0f, 0);
		m_transforms.m_position = position();
		m_transforms.m_quaternion = glm::quatLookAt(direction, up);
		m_transforms.m_scale = get_scale();

		sendDelayedMessage(gaMessage::WANT_TO_MOVE,
			gaMessage::Flag::WANT_TO_MOVE_FALL,
			&m_transforms);

		break; }
	}

	gaEntity::dispatchMessage(message);
}

gaActor::~gaActor()
{
}
