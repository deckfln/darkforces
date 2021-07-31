#pragma once

#include <array>
#include <glm/vec3.hpp>

#include "../framework/controls/fwOrbitControl.h"
#include "../framework/fwCamera.h"

class myDarkForces;

namespace Debugger {
	class Debug {
		bool m_debug = false;	// entered debug mode
		bool m_framebyframe = false;

		// flight recorder v2
		int m_frame = 0;	// current frame for replay

		// game controller
		fwControl* m_gameControl = nullptr;

		// debug controller
		fwOrbitControl* m_control = nullptr;

		// flight recorder V1
		std::array<glm::vec3, 3000> m_recorder = {};
		int m_recorder_start = 0;
		int m_recorder_end = 0;
		int m_recorder_len = 0;
		bool m_replay = false;
		void loadRecorderV1(void);
		void playRecorderV1(void);


	public:
		Debug();
		void debugMode(bool mode);
		void render(myDarkForces *dark);
		~Debug();
	};
}

extern Debugger::Debug g_Debugger;