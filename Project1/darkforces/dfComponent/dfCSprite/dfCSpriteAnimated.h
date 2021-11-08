#pragma once

#include <stack>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

#include "../dfCSprite.h"

#include "../../framework/fwMesh.h"

class dfWAX;

namespace DarkForces {
	namespace Component {
		class SpriteAnimated : public Sprite
		{
			dfState m_state = dfState::NONE;		// cached version of the state
			int32_t m_frame = 0;					// current frame to display based on frameSpeed
			uint32_t m_nbframes = 0;				// number of frames in the loop
			glm::vec3 m_direction = glm::vec3(0);	// direction the object is looking to
			time_t m_lastFrame = 0;					// time of the last animation frame
			time_t m_currentFrame = 0;				// time of the current animation frame
			bool m_animated = false;				// is the object running an animation loop ?
			fwMesh* m_view = nullptr;

#ifdef _DEBUG
			bool m_directionVector = false;			// add a vector to show the direction the sprite is facing
			bool m_debug = false;

			void directionVector(void);				// add/remove a vector mesh
#endif
			bool update(time_t t);					// update based on timer

			void onSetAnimation(gaMessage* message);		// activate the animation of a given state

		protected:
			bool m_loopAnimation = true;			// animations are looping (default)

		public:
			SpriteAnimated();
			SpriteAnimated(dfWAX* wax, float ambient);
			SpriteAnimated(const std::string& model, float ambient);

			void rotation(const glm::vec3& rotation);

			void loop(bool l) { m_loopAnimation = l; };
			bool loop(void) { return m_loopAnimation; };

			bool update(glm::vec3* position,
				glm::vec4* texture,
				glm::vec3* direction) override;

			void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger

			~SpriteAnimated(void);
		};
	}
}
