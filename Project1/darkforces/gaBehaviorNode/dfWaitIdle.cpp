#include "dfWaitIdle.h"

#include "../dfConfig.h"

DarkForces::Behavior::WaitIdle::WaitIdle(const char* name):
	BehaviorNode(name)
{
}

void DarkForces::Behavior::WaitIdle::dispatchMessage(gaMessage* message, Action* r)
{
	if (message->m_action == DarkForces::Message::HIT_BULLET) {
		return startChild(r, 0, nullptr);
	}

	execute(r);
}
