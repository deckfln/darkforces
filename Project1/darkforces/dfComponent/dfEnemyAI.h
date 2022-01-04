#pragma once

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"

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