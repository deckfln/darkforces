#include "dfBFindElevator.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/Physics.h"

#include "../dfComponent.h"
#include "../dfLogicTrigger.h"
#include "../dfComponent/InfElevator.h"

#include <tinyxml2.h>

DarkForces::Behavior::FindElevator::FindElevator(const char* name):
	GameEngine::BehaviorNode(name)
{
}

GameEngine::BehaviorNode* DarkForces::Behavior::FindElevator::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::FindElevator* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::FindElevator*>(p);
	}
	else {
		cl = new DarkForces::Behavior::FindElevator(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	return cl;
}

/**
 * generate from XML
 */
GameEngine::BehaviorNode* DarkForces::Behavior::FindElevator::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::FindElevator* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::FindElevator(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::FindElevator*>(used);
	}

	GameEngine::BehaviorNode::create(name, element, node);

	return node;
}

/**
 * 
 */
void DarkForces::Behavior::FindElevator::init(void* data)
{
	// the move_to node failed, it probably collided with something
	struct GameEngine::Physics::CollisionList& collidedList = m_tree->blackboard<struct GameEngine::Physics::CollisionList>("lastCollision");
	if (collidedList.size == 0) {
		m_status = GameEngine::BehaviorNode::Status::FAILED;
		return;
	}

	DarkForces::Component::InfElevator* elevator = nullptr;
	gaEntity* collided;
	for (auto i = 0; i < collidedList.size; i++) {
		collided = collidedList.entities[i];
		elevator = dynamic_cast<DarkForces::Component::InfElevator*>(collided->findComponent(DF_COMPONENT_INF_ELEVATOR));

		if (elevator != nullptr) {
			break;
		}
	}

	if (elevator == nullptr) {
		m_status = GameEngine::BehaviorNode::Status::FAILED;
		return;
	}

	m_tree->blackboard<DarkForces::Component::InfElevator>("wait_elevator", elevator);
	m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
}