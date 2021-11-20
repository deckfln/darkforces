#pragma once

#include "../gaComponent.h"

namespace GameEngine {
	namespace Component {
		class AIPerception : public gaComponent {
			// audio perception
			bool m_audio = false;

			// view perception
			bool m_view = false;
			float m_distance=0.0f;		// view distance
			float m_angle = 0.0f;		// angle of the vision cone
		public:
			AIPerception(bool view=false, bool audio=false, float distance=0, float angle=0);

			// getter/setter
			inline float distance(void) { return m_distance; };
			inline void distance(float d) { m_distance = d; };
			inline float angle(void) { return m_angle; };
			inline void audio(void) { m_audio = true; registerEvents(); };
			inline void view(void) { m_view = true; registerEvents(); };
			void registerEvents(void);

			~AIPerception();
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}