#include "dfBSetVar.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../dfComponent.h"
#include "../dfSector.h"
#include "../dfLogicTrigger.h"
#include "../dfComponent/InfElevator.h"
#include "../dfComponent/dfCTrigger.h"
#include "../dfComponent/InfElevator.h"
#include "../dfComponent/dfSign.h"

#include <tinyxml2.h>

static const char* g_className = "DarkForces:SetVar";

DarkForces::Behavior::SetVar::SetVar(const char* name):
	GameEngine::Behavior::SetVar(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* DarkForces::Behavior::SetVar::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::SetVar* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::SetVar*>(p);
	}
	else {
		cl = new DarkForces::Behavior::SetVar(m_name);
	}
	GameEngine::Behavior::SetVar::clone(cl);
	return cl;
}

/**
 * generate from XML
 */
GameEngine::BehaviorNode* DarkForces::Behavior::SetVar::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::SetVar* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::SetVar(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::SetVar*>(used);
	}

	GameEngine::Behavior::SetVar::create(name, element, node);

	return node;
}

/**
 * 
 */
void DarkForces::Behavior::SetVar::init(void* data)
{
	if (m_value.var() == "elevator.triggers.count") {
		// we are on a natural move, to the elevator can be activated
		// test all triggers of the object
		Component::InfElevator* elevator = m_tree->blackboard().pGet<Component::InfElevator>("wait_elevator", GameEngine::Variable::Type::PTR);

		if (elevator == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		// convert the Trigger elevator to the parent entity
		const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();

		m_variable.set(m_tree, (int32_t)cTriggers.size());
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}
	else if (m_value.var() == "elevator.triggers[trigger].position") {
		// we are on a natural move, to the elevator can be activated
		// test all triggers of the objectwait
		Component::InfElevator* elevator = m_tree->blackboard().pGet<Component::InfElevator>("wait_elevator", GameEngine::Variable::Type::PTR);
		int32_t current_trigger = m_tree->blackboard().get<int32_t>("trigger", GameEngine::Variable::Type::INT32);

		if (elevator == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		// convert the Trigger elevator to the parent entity
		const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();
		gaEntity* targetTrigger = cTriggers[current_trigger]->entity();

		glm::vec3 v3 = targetTrigger->position();

		// if the trigger is a dfSign, move in front of the object, on ON the object
		Component::Sign* sign = dynamic_cast<Component::Sign*>(targetTrigger->findComponent(DF_COMPONENT_SIGN));
		if (sign) {
			v3 += sign->normal() * m_entity->radius();
		}

		m_variable.set(m_tree, v3);
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}
	else if (m_value.var() == "elevator.triggers[trigger].name") {
		// we are on a natural move, to the elevator can be activated
		// test all triggers of the object
		Component::InfElevator* elevator = m_tree->blackboard().pGet<Component::InfElevator>("wait_elevator", GameEngine::Variable::Type::PTR);
		int32_t current_trigger = m_tree->blackboard().get<int32_t>("trigger", GameEngine::Variable::Type::INT32);

		if (elevator == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		// convert the Trigger elevator to the parent entity
		const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();
		gaEntity* targetTrigger = cTriggers[current_trigger]->entity();

		m_variable.set(m_tree, targetTrigger->name());
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}
	else if (m_value.var() == "findElevator") {
		// the move_to node failed, it probably collided with something
		struct GameEngine::Physics::CollisionList& collidedList = m_tree->blackboard().get<struct GameEngine::Physics::CollisionList>("lastCollision", GameEngine::Variable::Type::OBJECT);
		if (collidedList.size == 0) {
			m_status = GameEngine::BehaviorNode::Status::FAILED;
			return;
		}

		DarkForces::Component::InfElevator* elevator = nullptr;
		gaEntity* collided;
		for (size_t i = 0; i < collidedList.size; i++) {
			collided = collidedList.entities[i];
			//printf("DarkForces::Behavior::SetVar::findElevator=%s\n", collided->name().c_str());

			elevator = dynamic_cast<DarkForces::Component::InfElevator*>(collided->findComponent(DF_COMPONENT_INF_ELEVATOR));

			if (elevator != nullptr) {
				break;
			}
		}

		if (elevator == nullptr) {
			m_status = GameEngine::BehaviorNode::Status::FAILED;
			return;
		}

		m_variable.set(m_tree, elevator);
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}
	else if (m_value.var() == "playerLastPosition") {
		// this could be the real position (player is visible)
		// or the last known position (player is hidden)
		std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard().get<std::deque<glm::vec3>>("player_last_positions", GameEngine::Variable::Type::OBJECT);
		if (playerLastPositions.size() == 0) {
			m_status = Status::FAILED;
			return;
		}

		glm::vec3 move2 = playerLastPositions.back();

		// stop 8 clicks away from the player
		glm::vec3 v = move2 - m_entity->position();
		float l = glm::length(v) - m_entity->radius() * 8.0f;
		if (l < 0) {
			m_tree->blackboard().set<bool>("nearby_player", true, GameEngine::Variable::Type::BOOL);
			m_status = Status::SUCCESSED;
			return;
		}

		;

		v = glm::normalize(v) * l;
		if (glm::length(v) < m_entity->radius()) {
			m_tree->blackboard().set<bool>("nearby_player", true, GameEngine::Variable::Type::BOOL);
			m_status = Status::SUCCESSED;
			return;
		}

		v += m_entity->position();
		//m_target.y = m_entity->position().y;

		m_tree->blackboard().set<bool>("nearby_player", false, GameEngine::Variable::Type::BOOL);

		m_variable.set(m_tree, v);
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}
	else if (m_value.var() == "playerEstimatedPosition()") {
		/*
		static int pi = 0;
		if (m_entity->name() == "OFFCFIN.WAX(21)") {
			pi++;
			if (pi == 2) {
				m_tree->blackboard<bool>("debug_satnave", true);
			}
		}
		*/
		glm::vec3 v;

		// Pick the last 2 known positions and run the entity along the axe up to a wall
		std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard().get<std::deque<glm::vec3>>("player_last_positions", GameEngine::Variable::Type::OBJECT);
		size_t size = playerLastPositions.size();
		glm::vec2 direction;

		if (size < 2) {
			// if we don't have enough position of the player, were and when did we last heard a blaster shot

			v = m_tree->blackboard().get<glm::vec3>("last_heard_sound", GameEngine::Variable::Type::VEC3);
			if (v == glm::vec3(0)) {
				m_status = Status::FAILED;
				return;
			}
		}
		else {
			// the player may have been seen twice at the same position, so find a different position, but only go back a bit
			glm::vec3 p1 = playerLastPositions.at(--size);
			glm::vec3 p2 = playerLastPositions.at(--size);
			int i = 5;

			while (p1.x == p2.x && p1.z == p2.z && size > 0) {
				p2 = playerLastPositions.at(--size);
			}
			if (size <= 0) {
				// player as static all the time
				v = playerLastPositions.back();
			}
			else {
				glm::vec2 p1d(p1.x, p1.z);
				glm::vec2 p2d(p2.x, p2.z);

	#ifdef _DEBUG
				if (p1d == p2d) {
					__debugbreak();
				}
	#endif

				direction = glm::normalize(p1d - p2d);
				v = glm::vec3(direction.x + p1d.x, m_entity->position().y, direction.y + p1d.y);
			}

		}

		m_variable.set(m_tree, v);
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}

	GameEngine::Behavior::SetVar::init(data);
}