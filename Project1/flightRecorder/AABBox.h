#pragma once

/**
 * record the state of an AABBox
 */
#include <glm/vec3.hpp>
#include "classes.h"
#include "../framework/fwAABBox.h"

namespace flightRecorder {
	struct AABBox {
		TYPE classID = TYPE::AABBOX;
		glm::vec3 p;
		glm::vec3 p1;
	};
}
