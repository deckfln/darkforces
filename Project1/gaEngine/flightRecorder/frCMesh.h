#pragma once

#include <stdint.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../flightRecorder/Object3D.h"

namespace flighRecorder {
	namespace GameEngine {
		struct CMesh {
			uint32_t size;
			flightRecorder::Object3D object3D;
		};
	}
}