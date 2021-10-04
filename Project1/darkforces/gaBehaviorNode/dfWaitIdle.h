#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class WaitIdle : public GameEngine::BehaviorNode
		{
		public:
			WaitIdle(const char* name);
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
		};
	}
}
