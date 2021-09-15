#pragma once

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaBehaviorNode/gaBNSatNav.h"

namespace DarkForces {
	namespace Component {
		class MoveEnemy : public GameEngine::Component::BehaviorTree
		{
			GameEngine::Behavior::SatNav m_satnav;
		public:
			MoveEnemy();
		};
	}
}