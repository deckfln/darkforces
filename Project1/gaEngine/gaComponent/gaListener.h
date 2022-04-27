#pragma once

#include "../../alEngine/alSource.h"
#include "../gaComponent.h"

namespace GameEngine {
	namespace Component {
		class Listener : public gaComponent {
			static alSource m_source;							// source to play the sound for real
			void onHearSound(gaMessage* message);
			void onHearStop(gaMessage* message);
		public:
			Listener(void);
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
#ifdef _DEBUG
			void debugGUIinline(void) override;					// display the component in the debugger
#endif
		};
	}
}