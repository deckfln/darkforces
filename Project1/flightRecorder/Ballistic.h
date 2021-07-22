#pragma once

namespace flightRecorder {
	struct Ballistic {
		TYPE classID;

		char name[64];
		time_t physic_time_elpased;
		glm::mat3x3 physic;
		bool inUse;
	};
}