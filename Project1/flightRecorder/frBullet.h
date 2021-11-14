#pragma once

#include "Entity.h"

namespace flightRecorder {
	struct dfBullet {
		struct Entity entity;
		glm::vec3 direction;
		uint32_t damage;
	};
}
