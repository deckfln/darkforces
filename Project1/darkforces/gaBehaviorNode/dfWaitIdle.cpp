#include "dfWaitIdle.h"

#include "../dfConfig.h"
#include "../dfObject.h"

DarkForces::Behavior::WaitIdle::WaitIdle(const char* name):
	BehaviorNode(name)
{
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
	if (message->m_action == DarkForces::Message::HIT_BULLET) {
		m_original = m_entity->position();
		return startChild(r, 0, nullptr);
	}

	GameEngine::BehaviorNode::execute(r);
}

void DarkForces::Behavior::WaitIdle::execute(Action* r)
{
	if (m_runningChild >= 0) {
		switch (m_runningChild) {
		case 0:
			// lost the player, so move back to original position
			return startChild(r, 1, &m_original);
			break;

		case 1:
			// moved back to original position
			m_runningChild = -1;
			return succeeded(r);
			break;
		}
	}
	else {
		GameEngine::BehaviorNode::execute(r);
	}
}
