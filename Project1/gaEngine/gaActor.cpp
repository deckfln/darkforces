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
	translate(feet);

	m_physical = true;
	m_canStep = true;
	m_step = 0.2061f;

	m_modelAABB = fwAABBox(cylinder);
	updateWorldAABB();
	m_hasCollider = true;
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

	sendMessage(
		m_name,
		gaMessage::Action::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_FALL,
		&m_transforms);

	return false;
}

/**
 * trigger a jump
 */
void gaActor::jump(const glm::vec3& velocity)
{
	glm::vec3 direction = velocity * m_speed;
	direction.y = c_jump;

	m_physic[0][0] = 0;			m_physic[1][0] = direction.x/20.0f;		m_physic[2][0] = m_cylinder.position().x;
	m_physic[0][1] = c_gravity; m_physic[1][1] = direction.y/20.0f;		m_physic[2][1] = m_cylinder.position().y;
	m_physic[0][2] = 0;			m_physic[1][2] = direction.z/20.0f;		m_physic[2][2] = m_cylinder.position().z;

	//m_physic_time_elpased = 33;
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

		sendDelayedMessageToWorld(gaMessage::WANT_TO_MOVE,
			gaMessage::Flag::WANT_TO_MOVE_FALL,
			&m_transforms);

		break; }
	case gaMessage::CONTROLLER:
		moveTo(message->m_value, *(glm::vec3*)message->m_extra);
		break;
	}

	gaEntity::dispatchMessage(message);
}

gaActor::~gaActor()
{
}
