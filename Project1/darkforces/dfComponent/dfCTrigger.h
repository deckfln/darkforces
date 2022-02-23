#pragma once

#include "../../gaEngine/gaComponent.h"
#include "../dfConfig.h"

namespace DarkForces{
	namespace Component {
		class Trigger : public gaComponent {
			std::string m_key;									// needed key to activate the elevator

			void onTrigger(gaMessage* message);
		public:
			Trigger(const std::string& key="");
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}