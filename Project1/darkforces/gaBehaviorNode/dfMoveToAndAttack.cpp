#include "dfMoveToAndAttack.h"

#include <imgui.h>

#include "../../darkforces/dfObject.h"
#include "../dfComponent/dfMoveEnemy.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::MoveToAndAttack::MoveToAndAttack(const char* name):
	GameEngine::Behavior::Loop(name)
{
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::MoveToAndAttack::init(void* data)
{
	m_status = Status::RUNNING;
	m_runningChild = -1;
}

/**
 * let a parent take a decision with it's current running child
 */
void DarkForces::Behavior::MoveToAndAttack::execute(Action* r)
{
	if (!static_cast<DarkForces::Component::MoveEnemy*>(m_tree)->locatePlayer()) {
		return failed(r);
	}

	GameEngine::Behavior::Loop::execute(r);
}
