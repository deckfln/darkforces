#pragma once
#include <time.h>
#include "classes.h"

namespace flightRecorder {
	struct Message {
		TYPE classid = TYPE::MESSAGE;
		char server[64];	// from
		char client[64];	// to

		int action;
		int value;
		float fvalue;
		glm::vec3 v3value;
		time_t delta;		// time since the last frame
	};
}