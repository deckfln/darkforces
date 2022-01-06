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

static const char* g_className = "DarkForces:Alarm";

DarkForces::Behavior::Activate::Activate(const char *name):
	GameEngine::BehaviorNode(name)
{
	m_className = g_className;
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

//------------------------------

void DarkForces::Behavior::Activate::init(void *data)
{
	// we are on a natural move, to the elevator can be activated
	// test all triggers of the object

	const std::string& trigger = m_tree->blackboard().get<std::string>(m_variable, GameEngine::Variable::Type::STRING);

	m_entity->sendMessage(trigger, DarkForces::Message::TRIGGER);

	// broadcast the end of the move (for animation)
	m_entity->sendMessage(gaMessage::END_MOVE);

	// give time for the door to start opening, so check on next frame
	g_gaWorld.sendMessageDelayed(m_entity->name(), m_entity->name(), gaMessage::Action::TICK, 0, nullptr);

	GameEngine::BehaviorNode::init(data);
}

/**
 *
 */
void DarkForces::Behavior::Activate::dispatchMessage(gaMessage* message, Action* r)
{
	if (message->m_action == gaMessage::Action::TICK) {
		return succeeded(r);
	}

	GameEngine::BehaviorNode::dispatchMessage(message, r);
}

//---------------------------

/**
 * Debugger
 */
void DarkForces::Behavior::Activate::debugGUInode(void)
{
	const std::string& trigger = m_tree->blackboard().get<std::string>(m_variable, GameEngine::Variable::Type::STRING);

	if (trigger != "") {
		ImGui::Text("Activate: %s", trigger.c_str());
	}
}
