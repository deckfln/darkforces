#pragma once

#include <glm/vec3.hpp>

#include "../dfConfig.h"

#include "../../gaEngine/gaComponent.h"

namespace DarkForces {
	class AIMouseBot : public gaComponent
	{
		glm::vec3 m_direction = glm::vec3(1.0, 0, 1.0);
		float m_alpha = 0;								// rotation angle for the direction
		int m_animation_time = 0;						// time left to move in the direction
		uint32_t m_eekDelay = 100;						// frame left until next 'eeeek'
		bool m_active = true;							// is AI active
		GameEngine::Transform* m_transforms = nullptr;	// transforms to move the object
		uint32_t m_frame=0;								// reference to the last frame we received a message from
		void tryToMove(void);

	public:
		AIMouseBot();

		void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation

		// debugger
		void debugGUIinline(void) override;					// display the component in the debugger

			// flight recorder
		inline uint32_t recordSize(void);
		uint32_t recordState(void* r);
		uint32_t loadState(void* r);
	};
}
