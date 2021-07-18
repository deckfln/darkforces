#pragma once

#include <array>
#include <glm/vec3.hpp>

class myDarkForces;

namespace Debugger {
	class Debug {
		bool m_debug = false;	// entered debug mode
		bool m_framebyframe = false;

		// flight recorder V1
		std::array<glm::vec3, 3000> m_recorder;
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