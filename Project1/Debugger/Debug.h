#pragma once

class myDarkForces;

namespace Debugger {
	class Debug {
		bool m_debug = false;	// entered debug mode
		int m_frame = 0;		// frame # during a replay
	public:
		Debug();
		void debugMode(bool mode);
		void render(myDarkForces *dark);
		~Debug();
	};
}

extern Debugger::Debug g_Debugger;