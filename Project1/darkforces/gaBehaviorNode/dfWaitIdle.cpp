#include "dfWaitIdle.h"

#include "../dfConfig.h"
#include "../dfObject.h"

#include "../dfComponent/dfEnemyAI.h"

DarkForces::Behavior::WaitIdle::WaitIdle(const char* name):
	GameEngine::Behavior::Sequence(name)
{
	m_condition = Condition::EXIT_AT_END;
}

/**
 * re-activated the STILL status when the node gets re-activated
 */ 
void DarkForces::Behavior::WaitIdle::activated(void)
{
	m_entity->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);
}


void DarkForces::Behavior::WaitIdle::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
		/*
	case gaMessage::Action::BULLET_HIT: {
		m_original = m_entity->position();
		m_data = &m_original;
		m_runningChild = 0;

		// reset the list of player positions
		std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");
		playerLastPositions->clear();

		dynamic_cast<DarkForces::Component::EnemyAI*>(m_tree)->locatePlayer();
		return startChild(r, 0, m_data); }

	case gaMessage::Action::VIEW:
	case gaMessage::Action::HEAR_SOUND:
		m_original = m_entity->position();
		m_data = &m_original;
		m_runningChild = 0;
		return startChild(r, 0, m_data);
		*/
	}

	GameEngine::BehaviorNode::execute(r);
}