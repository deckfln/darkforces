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
		virtual bool dispatchMessage(gaEntity* to, gaMessage* message) = 0;
		~Plugin();
	};
}