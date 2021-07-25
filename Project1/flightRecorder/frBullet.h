#pragma once

#include "Entity.h"

namespace flightRecorder {
	struct dfBullet {
		struct Entity entity;
		glm::vec3 m_direction;
	};
}

extern void* frCreate_Bullet(void*);