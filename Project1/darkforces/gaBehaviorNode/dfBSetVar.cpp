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

	if (node->m_type == Type::VAR) {
		if (node->m_svalue == "elevator.triggers.count") {
			node->m_type = Type::INT32;
		}
		else if (node->m_svalue == "elevator.triggers[trigger].position") {
			node->m_type = Type::VEC3;
		}
		else if (node->m_svalue == "elevator.triggers[trigger].name") {
			node->m_type = Type::STRING;
		}
	}
	return node;
}

/**
 * 
 */
void DarkForces::Behavior::SetVar::init(void* data)
{
	if (m_svalue == "elevator.triggers.count") {
		// we are on a natural move, to the elevator can be activated
		// test all triggers of the object
		Component::InfElevator* elevator = m_tree->pBlackboard<Component::InfElevator>("wait_elevator");

		if (elevator == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		// convert the Trigger elevator to the parent entity
		const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();

		m_ivalue = cTriggers.size();
	}
	else if (m_svalue == "elevator.triggers[trigger].position") {
		// we are on a natural move, to the elevator can be activated
		// test all triggers of the objectwait
		Component::InfElevator* elevator = m_tree->pBlackboard<Component::InfElevator>("wait_elevator");
		int32_t current_trigger = m_tree->blackboard<int32_t>("trigger");

		if (elevator == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		// convert the Trigger elevator to the parent entity
		const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();
		gaEntity* targetTrigger = cTriggers[current_trigger]->entity();

		m_v3value = targetTrigger->position();

		// if the trigger is a dfSign, move in front of the object, on ON the object
		Component::Sign* sign = dynamic_cast<Component::Sign*>(targetTrigger->findComponent(DF_COMPONENT_SIGN));
		if (sign) {
			m_v3value += sign->normal();
		}
	}
	else if (m_svalue == "elevator.triggers[trigger].name") {
		// we are on a natural move, to the elevator can be activated
		// test all triggers of the object
		Component::InfElevator* elevator = m_tree->pBlackboard<Component::InfElevator>("wait_elevator");
		int32_t current_trigger = m_tree->blackboard<int32_t>("trigger");

		if (elevator == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		// convert the Trigger elevator to the parent entity
		const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();
		gaEntity* targetTrigger = cTriggers[current_trigger]->entity();

		m_svalue = targetTrigger->name();
	}

	GameEngine::Behavior::SetVar::init(data);
}