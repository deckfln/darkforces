#pragma once

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaBehaviorNode/gaBNSatNav.h"
#include "../../gaEngine/gaBehaviorNode/gaBehaviorLoop.h"

#include "../gaBehaviorNode/dfMoveEnemyTo.h"
#include "../gaBehaviorNode/dfWaitDoor.h"
#include "../gaBehaviorNode/dfOpenDoor.h"
#include "../gaBehaviorNode/dfGotoTrigger.h"
#include "../gaBehaviorNode/dfWaitIdle.h"
#include "../gaBehaviorNode/dfMove2player.h"
#include "../gaBehaviorNode/dfFire2Player.h"

namespace DarkForces {
	namespace Component {
		class MoveEnemy : public GameEngine::Component::BehaviorTree
		{
			Behavior::WaitIdle m_waitIdle = Behavior::WaitIdle("wait_idle");
				GameEngine::Behavior::Loop m_attack = GameEngine::Behavior::Loop("attack");
					Behavior::Move2Player m_move2player = Behavior::Move2Player("move toward player");
						GameEngine::Behavior::MoveTo m_move2 = GameEngine::Behavior::MoveTo("move to waypoints");
					Behavior::Fire2Player m_shootPlayer = Behavior::Fire2Player("shoot player");

			GameEngine::Behavior::SatNav m_satnav = GameEngine::Behavior::SatNav("go to destination");
			Behavior::MoveEnemyTo m_move_to = Behavior::MoveEnemyTo("move to destination");
			Behavior::WaitDoor m_wait_door = Behavior::WaitDoor("wait for door to open");
			Behavior::OpenDoor m_open_door = Behavior::OpenDoor("go to, open and wait door to open");
			Behavior::GotoTrigger m_goto_trigger = Behavior::GotoTrigger("try to reach each trigger");
			GameEngine::Behavior::SatNav m_satnav_door = GameEngine::Behavior::SatNav("go to trigger");
			Behavior::WaitDoor m_wait_door_2 = Behavior::WaitDoor("wait for door to open");

		public:
			MoveEnemy();
		};
	}
}