#include "AIMouseBot.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include "../../config.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaEntity.h"

#include "../dfObject.h"

using namespace DarkForces;

AIMouseBot::AIMouseBot():
	gaComponent(DF_COMPONENT_AI)
{
	// find the sector the mousebot is running into
	m_direction = glm::normalize(m_direction);
	m_movement = m_direction * 0.01f;
}

/**
 *
 */
void AIMouseBot::tryToMove(void)
{
	m_direction = glm::rotateY(m_direction, m_alpha);

	// align the object to the direction
	//convert the direction vector to a quaternion
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right;
	glm::vec3 up;

	if (m_direction != glm::vec3(0)) {
		m_direction = glm::normalize(m_direction);
		right = glm::normalize(glm::cross(_up, m_direction));
		up = glm::cross(m_direction, right);
	}
	else {
		up = _up;
	}

	m_transforms->m_forward = m_direction * 0.06f;
	m_transforms->m_downward = glm::vec3(0, -1, 0);
	m_transforms->m_position = m_entity->position() + m_transforms->m_forward;
	m_transforms->m_quaternion = glm::quatLookAt(m_direction, up);
	m_transforms->m_scale = m_entity->get_scale();
	m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;

	m_entity->sendDelayedMessage(gaMessage::WANT_TO_MOVE, 
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL, 
		m_transforms);
}

/**
 *
 */
void AIMouseBot::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::WORLD_INSERT:
		m_transforms = m_entity->pTransform();
		// kick start the AI
		m_center = m_entity->position();
		m_alpha = (rand() / (float)RAND_MAX - 0.f) / 10.0f; // rotation angle to apply to the direction vector
		m_animation_time = rand() % (10 * 30);				// move 5s maximum using the same rotation angle

		tryToMove();
		break;

	case DarkForces::Message::HIT_BULLET:
		m_entity->sendMessage(DarkForces::Message::DEAD);
		break;

	case DarkForces::Message::DEAD:
		static_cast<dfObject*>(m_entity)->drop(dfLogic::DEAD_MOUSE);
		static_cast<dfObject*>(m_entity)->drop(dfLogic::ITEM_BATTERY);
		
		// 3D objects being registered in dfParserObject cannot be deleted, so move away
		m_entity->moveTo(glm::vec3(0));
		m_active = false;
		break;

	case gaMessage::Action::COLLIDE:
		if (message->m_value == gaMessage::Flag::TRAVERSE_ENTITY) {
			break;	// ignore non-physical objects
		}
		// PASS THROUGH
	case gaMessage::Action::WOULD_FALL:

		// second time we receive the same message inside the same frame
		// => we collided with a second (or third) object
		if (m_frame != message->m_frame) {
			m_frame = message->m_frame;
			m_direction = -m_direction;
		}
		else {
			return;
		}
		// PASS THROUGH

	case gaMessage::Action::MOVE:
		// move request was accepted, so trigger a new one
		if (!m_active) {
			break;
		}
		m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;

		// go for next movement
		if (--m_animation_time < 0) {
			// after the default delay, change the rotation angle
			m_alpha = (rand() / (float)RAND_MAX - 0.5f) / 10.0f;
			m_animation_time = rand() % (5 * 30);
		}

		tryToMove();
		break;
	}
}

AIMouseBot::~AIMouseBot()
{
}
