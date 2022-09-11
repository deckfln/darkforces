#pragma once

#include "../gaBehaviorPlugin.h"

namespace GameEngine {
	namespace Behavior {
		namespace Plugin {
			class View : public Base {
				bool onViewPlayer(GameEngine::Blackboard& blackboard, gaMessage*);						// player is viewed
				bool onNotViewPlayer(GameEngine::Blackboard& blackboard, gaMessage*);					// player is not viewed

			public:
				View(void);
				void dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage*) override;
				static Base* Create(void);
			};
		}
	}
}