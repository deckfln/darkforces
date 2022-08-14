#pragma once

#include "../../alEngine/alSource.h"
#include "../gaComponent.h"

namespace GameEngine {
	namespace Component {
		class Listener : public gaComponent {
			static alSource m_source;							// source to play the sound for real
			float m_source_loundness = -1.0f;
			glm::vec3 m_source_position = glm::vec3(0);
			void onHearSoundNext(gaMessage* message);			// sound in a list of sound
			void onHearSound(gaMessage* message);				// last sound of the batch
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