#pragma once

#include "../gaComponent.h"

namespace GameEngine {
	namespace Component {
		class AIPerception : public gaComponent {
			float m_distance=0.0f;		// view distance
			float m_angle = 0.0f;		// angle of the vision cone
		public:
			AIPerception(float distance=0, float angle=0);

			// getter/setter
			inline float distance(void) { return m_distance; };
			inline void distance(float d) { m_distance = d; };
			inline float angle(void) { return m_angle; };
			void registerEvents(void);

			~AIPerception();
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}