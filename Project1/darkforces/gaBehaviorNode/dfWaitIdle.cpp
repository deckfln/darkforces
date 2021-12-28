#include "dfWaitIdle.h"

#include <tinyxml2.h>

#include "../dfConfig.h"
#include "../dfObject.h"

#include "../dfComponent/dfEnemyAI.h"

DarkForces::Behavior::WaitIdle::WaitIdle(const char* name):
	GameEngine::BehaviorNode(name)
{
}

BehaviorNode* DarkForces::Behavior::WaitIdle::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::WaitIdle* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::WaitIdle(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::WaitIdle*>(used);
	}

	bool value = false;

	tinyxml2::XMLElement* exits = element->FirstChildElement("exits");
	if (exits) {
		tinyxml2::XMLElement* exit = exits->FirstChildElement("exit");

		while (exit != nullptr) {
			exit->QueryBoolAttribute("value", &value);
			node->m_exit[exit->GetText()] = value;
			exit = exit->NextSiblingElement("exit");
		}
	}

	return node;
}

/**
 * execute the node
 */
void DarkForces::Behavior::WaitIdle::execute(Action* r)
{
	if (conditionMet()) {
		return succeeded(r);
	}
	GameEngine::BehaviorNode::execute(r);
}


/**
 * check exit conditions
 */
bool DarkForces::Behavior::WaitIdle::conditionMet(void)
{
	bool *condition;
	for (auto& exit : m_exit) {
		condition = m_tree->blackboard<bool>(exit.first);
		if (condition && *condition == exit.second) {
			return true;
		}
	}

	return false;
}

/**
 * re-activated the STILL status when the node gets re-activated
 */ 
void DarkForces::Behavior::WaitIdle::activated(void)
{
	m_entity->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);

	// reset the list of player positions

	std::deque<glm::vec3>* playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>("player_last_positions");
	playerLastPositions->clear();
}

void DarkForces::Behavior::WaitIdle::dispatchMessage(gaMessage* message, Action* r)
{
	if (conditionMet()) {
		m_original = m_entity->position();
		m_tree->blackboard<glm::vec3>("static_position", &m_original);

		r->data = &m_original;
		return succeeded(r);
	}

	GameEngine::BehaviorNode::dispatchMessage(message, r);
}

/**
 *
void DarkForces::Behavior::WaitIdle::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case gaMessage::Action::BULLET_HIT: {
		m_original = m_entity->position();
		m_data = &m_original;
		m_runningChild = 0;

		return startChild(r, 0, m_data); }

	case gaMessage::Action::VIEW:
	case gaMessage::Action::HEAR_SOUND:
		m_original = m_entity->position();
		m_data = &m_original;
		m_runningChild = 0;
		return startChild(r, 0, m_data);
	}

	GameEngine::BehaviorNode::execute(r);
}
*/