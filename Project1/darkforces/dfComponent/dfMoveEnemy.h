#pragma once

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaBehaviorNode/gaBNSatNav.h"

#include "../gaBehaviorNode/dfMoveEnemyTo.h"
#include "../gaBehaviorNode/dfWaitDoor.h"
#include "../gaBehaviorNode/dfOpenDoor.h"
#include "../gaBehaviorNode/dfGotoTrigger.h"

namespace DarkForces {
	namespace Component {
		class MoveEnemy : public GameEngine::Component::BehaviorTree
		{
			GameEngine::Behavior::SatNav m_satnav;
			Behavior::MoveEnemyTo m_move_to;
			Behavior::WaitDoor m_wait_door;
			Behavior::OpenDoor m_open_door;
				Behavior::GotoTrigger m_goto_trigger;
					GameEngine::Behavior::SatNav m_satnav_door;
				Behavior::WaitDoor m_wait_door_2;

		public:
			MoveEnemy();
		};
	}
}