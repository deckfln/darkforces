#include "dfBSetVar.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../dfComponent.h"
#include "../dfSector.h"
#include "../dfLogicTrigger.h"
#include "../dfComponent/InfElevator.h"
#include "../dfComponent/Trigger.h"
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
	if (m_variable.var() == "elevator.triggers.count") {
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
	else if (m_variable.var() == "elevator.triggers[trigger].position") {
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
			v3 += sign->normal();
		}

		m_variable.set(m_tree, v3);
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		return;
	}
	else if (m_variable.var() == "elevator.triggers[trigger].name") {
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

	GameEngine::Behavior::SetVar::init(data);
}