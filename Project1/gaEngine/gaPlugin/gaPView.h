#pragma once

#include <map>
#include "../gaPlugin.h"

namespace GameEngine {
	namespace Plugins {
		class View : public GameEngine::Plugin {
			std::map<uint32_t, gaEntity*> m_views;				// entities that registered to receive visual events

		public:
			View(void);

			void beforeProcessing(void) override;				// execute the plugin before processing the message queue

			// (de)register entities for audio perceptions
			void registerViewEvents(gaEntity* entity);
			void deregisterViewEvents(gaEntity* entity);
		};
	}
}

extern GameEngine::Plugins::View g_gaViewEngine;