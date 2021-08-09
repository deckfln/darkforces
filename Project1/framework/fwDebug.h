#pragma once

#include <array>
#include <glm/vec3.hpp>

class fwApp;

namespace Framework {
	class Debug {
	protected:
		bool m_debug = false;	// entered debug mode
		bool m_requestDebug = false;
		fwApp* m_app = nullptr;

	public:
		Debug(fwApp* app);
		void debugMode(bool mode);
		virtual void render(void);
		~Debug();
	};
}