#include "dfWaitIdle.h"

#include <tinyxml2.h>

#include "../dfConfig.h"
#include "../dfObject.h"

#include "../dfComponent/dfEnemyAI.h"

static const char* g_className = "DarkForces:waitIdle";

DarkForces::Behavior::WaitIdle::WaitIdle(const char* name):
	GameEngine::BehaviorNode(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* DarkForces::Behavior::WaitIdle::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::WaitIdle* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::WaitIdle*>(p);
	}
	else {
		cl = new DarkForces::Behavior::WaitIdle(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	return cl;
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

	GameEngine::BehaviorNode::create(name, element, node);

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
 * re-activated the STILL status when the node gets re-activated
 */ 
void DarkForces::Behavior::WaitIdle::activated(void)
{
	// reset the list of player positions
	std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard().get<std::deque<glm::vec3>>("player_last_positions", GameEngine::Variable::Type::OBJECT);
	playerLastPositions.clear();
}

void DarkForces::Behavior::WaitIdle::dispatchMessage(gaMessage* message, Action* r)
{
	if (conditionMet()) {
		m_tree->blackboard().set<glm::vec3>("static_position", m_entity->position(), GameEngine::Variable::Type::VEC3);

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