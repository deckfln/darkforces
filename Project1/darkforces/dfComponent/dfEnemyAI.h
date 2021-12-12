#pragma once

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaBehaviorNode/gaBNSatNav.h"
#include "../../gaEngine/gaBehaviorNode/gaBehaviorLoop.h"
#include "../../gaEngine/gaBehaviorNode/gaBehaviorSound.h"

#include "../gaBehaviorNode/dfMoveEnemyTo.h"
#include "../gaBehaviorNode/dfWaitDoor.h"
#include "../gaBehaviorNode/dfOpenDoor.h"
#include "../gaBehaviorNode/dfGotoTrigger.h"
#include "../gaBehaviorNode/dfWaitIdle.h"
#include "../gaBehaviorNode/dfMove2player.h"
#include "../gaBehaviorNode/dfFire2Player.h"
#include "../gaBehaviorNode/dfAttackPlayer.h"
#include "../gaBehaviorNode/dfMoveToAndAttack.h"
#include "../gaBehaviorNode/dfTrackPlayer.h"

namespace DarkForces {
	namespace Component {
		class EnemyAI : public GameEngine::Component::BehaviorTree
		{
			gaEntity* m_player = nullptr;						// player entity
			glm::vec3 m_lastPlayerView;
			uint32_t m_lastPlayerViewFrame=0;					// frame ID when the player was seen
			uint32_t m_currentFrame=0;
			bool m_discardMessages = false;						// shall we handle the incoming messages

		public:
			EnemyAI();

			void dispatchMessage(gaMessage* message) override;

			bool onDying(gaMessage*);

			void addSound(const std::string& file, uint32_t);	// add sounds to 'tease the player'
		};
	}
}