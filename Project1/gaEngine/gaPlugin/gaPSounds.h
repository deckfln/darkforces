#pragma once

#include <map>
#include "../gaPlugin.h"

namespace GameEngine {
	namespace Plugins {
		class Sounds : public GameEngine::Plugin {
			std::map<uint32_t, gaEntity*> m_hear;				// entities that registered to receive audio events

			void onPropagateSound(gaEntity* to, gaMessage* message);
		public:
			Sounds(void);
			bool dispatchMessage(gaEntity* to, gaMessage* message) override;

			// (de)register entities for audio perceptions
			void registerHearEvents(gaEntity* entity);
			void deRegisterHearEvents(gaEntity* entity);
		};
	}
}

extern GameEngine::Plugins::Sounds g_gaSoundEngine;