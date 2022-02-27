#include <map>
#include "dfBState.h"
#include "../../gaEngine/gaEntity.h"
#include "../dfMessage.h"
#include "../dfObject.h"


static std::map<std::string, uint32_t> g_states = {
	{"ENEMY_STAY_STILL", (uint32_t)dfState::ENEMY_STAY_STILL}
};
static const char* g_className = "DarkForces:State";

/**
 *
 */
DarkForces::Behavior::State::State(const char* name) :
	GameEngine::BehaviorNode(name)
{
	m_className = g_className;
}

/**
 *
 */
BehaviorNode* DarkForces::Behavior::State::clone(GameEngine::BehaviorNode* p)
{
	State* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::State*>(p);
	}
	else {
		cl = new DarkForces::Behavior::State(m_name);
	}
	cl->m_state = m_state;
	GameEngine::BehaviorNode::clone(cl);
	return cl;
}

/**
 *
 */
BehaviorNode* DarkForces::Behavior::State::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	State* node;

	if (used == nullptr) {
		node = new State(name);
	}
	else {
		node = dynamic_cast<State*>(used);
	}

	GameEngine::BehaviorNode::create(name, element, node);

	// Get the exit conditions
	tinyxml2::XMLElement* xmlState = element->FirstChildElement("state");
	if (xmlState != nullptr) {
		const char* t = xmlState->GetText();
		if (g_states.count(t) > 0) {
			node->m_state = g_states[t];
		}
	}

	return node;
}

/**
 *
 */
void DarkForces::Behavior::State::execute(Action* r)
{
	m_entity->sendMessage(DarkForces::Message::STATE, m_state);
	return succeeded(r);
}