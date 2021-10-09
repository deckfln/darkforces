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
		return startChild(r, 0, nullptr);
	}

	execute(r);
}
