#pragma once

#include <array>
#include <glm/vec3.hpp>

#include "../framework/fwDebug.h"
#include "../framework/controls/fwOrbitControl.h"
#include "../framework/fwCamera.h"

class myDarkForces;

namespace GameEngine {
	class Debug : public Framework::Debug {
		bool m_framebyframe = false;

		// flight recorder v2
		int m_frame = 0;	// current frame for replay

		// game controller
		fwControl* m_gameControl = nullptr;

		// debug controller
		fwOrbitControl* m_control = nullptr;

		// list of entities to display
		std::map<std::string, void*> m_debugEntities;

		// flight recorder V1
		std::array<glm::vec3, 3000> m_recorder = {};
		int m_recorder_start = 0;
		int m_recorder_end = 0;
		int m_recorder_len = 0;
		bool m_replay = false;
		void loadRecorderV1(void);
		void playRecorderV1(void);

	public:
		Debug(myDarkForces *app);
		void render(void) override;
		~Debug();
	};
}