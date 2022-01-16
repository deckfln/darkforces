#pragma once

#include "../gaComponent.h"

#include "../../framework/fwMesh.h"

namespace GameEngine {
	namespace Component {
		class Actor : public gaComponent {
			glm::vec3 m_direction;								// the direction the parent entity is facing

#ifdef _DEBUG
			fwMesh* m_view = nullptr;
			bool m_directionVector = false;			// add a vector to show the direction the sprite is facing
			bool m_debug = false;

			void directionVector(void);				// add/remove a vector mesh
#endif

		public:
			Actor(void);

			//setter/getter
			inline const glm::vec3& direction(void) { return m_direction; };
			inline void direction(const glm::vec3& r) { m_direction = r; };

			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger

			// flight recorder status
			inline uint32_t recordSize(void);					// size of the component record
			uint32_t recordState(void* record);					// save the component state in a record
			uint32_t loadState(void* record);					// reload a component state from a record
		};
	}
}