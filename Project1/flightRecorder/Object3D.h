#pragma once

/**
 * State of an object3D
 */
#include <glm/vec3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include "classes.h"

namespace flightRecorder {
	struct Object3D {
		TYPE classID = TYPE::OBJECT3D;
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 rotation;
		glm::quat quaternion;
	};
}