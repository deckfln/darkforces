#include "gaActor.h"

#include <stdio.h>
#include <iostream>

#include "../config.h"
#include "../framework/fwCollision.h"

#include "gaBoundingBoxes.h"
#include "gaCollisionPoint.h"
#include "gaEntity.h"
#include "gaComponent/gaController.h"
#include "gaComponent/gaActiveProbe.h"
#include "World.h"

#include "../darkforces/dfLevel.h"

#include <GLFW/glfw3.h>

static const char* g_className = "gaActor";

/**
 *
 */
gaActor::gaActor(
	uint32_t mclass, 
	const std::string& name, 
	fwCylinder& cylinder,		// collision cylinder
	const glm::vec3& feet,			// position of the feet 
	float eyes, 
	float ankle) :
	gaEntity(GameEngine::ClassID::Actor | mclass, name),
	m_cylinder(cylinder),
	m_ankle(ankle),
	m_eyes(eyes)
{
	m_className = g_className;

	translate(feet);

	m_physical = true;
	m_canStep = true;
	m_step = 0.2061f;

	m_modelAABB = fwAABBox(cylinder);
	updateWorldAABB();
	m_hasCollider = true;
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);

	GameEngine::Component::ActiveProbe* probe = new GameEngine::Component::ActiveProbe();
	addComponent(probe, gaEntity::Flag::DELETE_AT_EXIT);

	addComponent(&m_listener);
}

/**
 *
 */
gaActor::gaActor(flightRecorder::Entity* record) :
	gaEntity(record)
{
	m_className = g_className;
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
	m_transforms.m_quaternion = glm::quatLookAt(glm::normalize(velocity), up);
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

		GameEngine::Component::Controller* controller = (GameEngine::Component::Controller*)findComponent(gaComponent::Controller);
		glm::vec3 direction = controller->direction();

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

		sendMessage(m_name, gaMessage::WANT_TO_MOVE,
			gaMessage::Flag::WANT_TO_MOVE_FALL,
			&m_transforms);

		break; }

	case gaMessage::CONTROLLER:
		moveTo(message->m_value, *(glm::vec3*)message->m_extra);
		break;

	case gaMessage::Action::LOOK_AT:
		m_direction = (*(glm::vec3*)message->m_extra);
		break;

	case gaMessage::KEY:
		switch (message->m_value) {
		case GLFW_KEY_S:
			g_gaWorld.suspendTimer();
			break;

		case GLFW_KEY_X: {
			glm::vec3* velocity = (glm::vec3 *)message->m_extra;
			glm::vec3 v0(velocity->x/3.0f, 0.2f, velocity->z/3.0f);
			g_gaPhysics.addBallistic(this, v0);
			break; }
		}
	}

	gaEntity::dispatchMessage(message);
}

/**
 * return a record of an actor state (for debug)
 */
uint32_t gaActor::recordState(void* r)
{
	gaEntity::recordState(r);
	flightRecorder::Actor* record = (flightRecorder::Actor*)r;
	record->entity.classID = flightRecorder::TYPE::ENTITY_ACTOR;
	record->entity.size = sizeof(flightRecorder::Actor);
	record->speed = m_speed;				// normal speed
	record->ankle = m_ankle;				// maximum step the actor can walk up
	record->eyes = m_eyes;					// position of the eyes (from the feet)
	record->step = m_step;					// how up/down can the actor step over
	record->physic = m_physic;
	record->animation_time = m_animation_time;// start of the physic driven movement

	return record->entity.size;
}

/**
 * reload an actor state from a record
 */
void gaActor::loadState(void* r)
{
	flightRecorder::Actor* record = (flightRecorder::Actor*)r;
	gaEntity::loadState(&record->entity);
	m_speed = record->speed;				// normal speed
	m_ankle = record->ankle;				// maximum step the actor can walk up
	m_eyes = record->eyes;					// position of the eyes (from the feet)
	m_step = record->step;					// how up/down can the actor step over
	m_physic = record->physic;
	m_animation_time = record->animation_time;// start of the physic driven movement
}

gaActor::~gaActor()
{
}
