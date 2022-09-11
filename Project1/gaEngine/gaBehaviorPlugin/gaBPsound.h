#pragma once

#include "../gaBehaviorPlugin.h"

namespace GameEngine {
	namespace Behavior {
		namespace Plugin {
			class Sounds : public Base {
				bool onHearSoundFirst(GameEngine::Blackboard& blackboard, gaMessage* message);
				bool onHearSoundNext(GameEngine::Blackboard& blackboard, gaMessage* message); // partial message hear
				bool onHearSoundLast(GameEngine::Blackboard& blackboard, gaMessage* message);

			public:
				Sounds(void);
				void dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage*) override;
				static Base* Create(void);
			};
		}
	}
}