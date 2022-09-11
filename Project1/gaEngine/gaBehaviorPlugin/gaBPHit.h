#pragma once

#include "../gaBehaviorPlugin.h"

namespace GameEngine {
	namespace Behavior {
		namespace Plugin {
			class Hit : public Base {
				bool onHit(GameEngine::Blackboard& blackboard, gaMessage* message);

			public:
				Hit(void);
				void dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage*) override;
				static Base* Create(void);
			};
		}
	}
}