#include "dfMoveToAndAttack.h"

#include <imgui.h>
#include <tinyxml2.h>

#include "../../darkforces/dfObject.h"
#include "../dfComponent/dfEnemyAI.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::MoveToAndAttack::MoveToAndAttack(const char* name):
	GameEngine::Behavior::Loop(name)
{
}

BehaviorNode* DarkForces::Behavior::MoveToAndAttack::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new DarkForces::Behavior::MoveToAndAttack(name);
}

/**
 * let a parent take a decision with it's current running child
 */
void DarkForces::Behavior::MoveToAndAttack::execute(Action* r)
{
	bool* b = m_tree->blackboard<bool>("player_visible");
	if (*b == false) {
		return failed(r);
	}

	GameEngine::Behavior::Loop::execute(r);
}

