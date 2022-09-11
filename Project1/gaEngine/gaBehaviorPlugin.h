#pragma once

#include <string>
#include "gaMessage.h"
#include "gaBlackboard.h"

namespace GameEngine {
	namespace Behavior {
		namespace Plugin {
			class Base {
				std::string m_name;
			public:
				typedef GameEngine::Behavior::Plugin::Base* (*pluginHandler)(void);

				inline Base(const std::string& name) : m_name(name) {}
				virtual void dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage* message) = 0;
				static Base *Create(void);
			};
		}
	}
}