#pragma once

/**
 * record the state of an AABBox
 */
#include <glm/vec3.hpp>
#include "classes.h"

namespace flightRecorder {
	struct AABBox {
		TYPE classID;
		glm::vec3 p;
		glm::vec3 p1;
	};
}
