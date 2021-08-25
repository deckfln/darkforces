#pragma once

#include "../../gaEngine/gaComponent.h"

namespace DarkForces{
	namespace Component {
		class Trigger : public gaComponent {
		public:
			Trigger(void);
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}