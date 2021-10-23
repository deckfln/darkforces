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
 * let a parent take a decision with it's current running child
 */
void DarkForces::Behavior::MoveToAndAttack::execute(Action* r)
{
	if (!static_cast<DarkForces::Component::MoveEnemy*>(m_tree)->viewPlayer()) {
		return failed(r);
	}

	GameEngine::Behavior::Loop::execute(r);
}
