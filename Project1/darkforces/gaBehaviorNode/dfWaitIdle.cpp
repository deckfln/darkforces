#include "dfWaitIdle.h"

#include "../dfConfig.h"
#include "../dfObject.h"

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
	case DarkForces::Message::HIT_BULLET:
		m_original = m_entity->position();
		m_data = &m_original;
		m_runningChild = 0;
		return startChild(r, 0, m_data);

	case gaMessage::Action::VIEW:
		m_original = m_entity->position();
		m_data = &m_original;
		m_runningChild = 0;
		return startChild(r, 0, m_data);
	}

	GameEngine::BehaviorNode::execute(r);
}