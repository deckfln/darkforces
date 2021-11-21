#pragma once

/*
 *  Framework for Wold plugins
 *     plugins can intercept messages or extend world features
 */

#include <vector>

#include "gaMessage.h"

namespace GameEngine {
	class Plugin {
		const char* m_plugin = nullptr;	// name of the plugin
	public:
		Plugin(const char* name);
		virtual void beforeProcessing(void) {};								// execute the plugin before processing the message queue
		virtual bool dispatchMessage(gaEntity* to, gaMessage* message) {
			return true;
		};																	// handle messages specific for the engine
		virtual bool interceptMessages(gaEntity* to, gaMessage* message) {
			return true;
		};																	// intercept messages dedicated to entities (with option to change them)
		~Plugin();
	};
}