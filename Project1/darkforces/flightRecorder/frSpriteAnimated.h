#pragma once

#include "../../flightRecorder/classes.h"
#include "frSprite.h"

namespace flightRecorder {
	namespace DarkForces {
		struct SpriteAnimated {
			struct flightRecorder::DarkForces::Sprite sprite;
			int32_t state;			// state of the object for WAX, unused for others
			bool loop;
			uint32_t frame;			// current frame to display based on frameSpeed
			glm::vec3 direction;	// direction the object is looking to
			time_t lastFrame;		// time of the last animation frame
			time_t currentFrame;	// time of the current animation frame
		};
	}
}
