#include "dfBActivate.h"

#include <tinyxml2.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"

#include "../dfComponent.h"
#include "../dfSector.h"
#include "../dfLogicTrigger.h"
#include "../dfComponent/Trigger.h"
#include "../dfComponent/InfElevator.h"
#include "../dfComponent/dfSign.h"

DarkForces::Behavior::Activate::Activate(const char *name):
	GameEngine::BehaviorNode(name)
{
}

GameEngine::BehaviorNode* DarkForces::Behavior::Activate::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::Activate* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::Activate*>(p);
	}
	else {
		cl = new DarkForces::Behavior::Activate(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_variable = m_variable;
	return cl;
}

BehaviorNode* DarkForces::Behavior::Activate::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::Activate* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::Activate(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::Activate*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	tinyxml2::XMLElement* xml = element->FirstChildElement("variable");
	if (xml) {
		node->m_variable = xml->GetText();
	}

	return node;
}

void DarkForces::Behavior::Activate::init(void *data)
{
	// we are on a natural move, to the elevator can be activated
	// test all triggers of the object

	const std::string& trigger = m_tree->blackboard<std::string>(m_variable);

	m_entity->sendMessage(trigger, DarkForces::Message::TRIGGER);

	// broadcast the end of the move (for animation)
	m_entity->sendMessage(gaMessage::END_MOVE);

	m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
}