#include "Debug.h"

#include "../myDarkForces.h"
#include "../gaEngine/gaActor.h"
#include "../gaEngine/World.h"

Debugger::Debug g_Debugger;

Debugger::Debug::Debug()
{
}

/**
 * Change debug state
 */
void Debugger::Debug::debugMode(bool mode)
{
	m_debug = mode;
}

/**
 * Render the interface
 */
void Debugger::Debug::render(myDarkForces *dark)
{
	glm::vec3 p = dark->m_player->position();
	ImGui::Begin("Debugger");                          // Create a window called "Hello, world!" and append into it.
	ImGui::Text("Player x:%.3f y:%.3f z:%.3f", p.x, p.y, p.z);

	if (m_debug) {
		if (ImGui::Button("Exit debug")) {
			m_debug = false;
			g_gaWorld.run();
		}

		ImGui::Button("Play"); ImGui::SameLine();
		ImGui::Button("Save"); ImGui::SameLine();
		ImGui::SliderInt("flightRecorder", &m_frame, 0, 300);
		g_gaWorld.renderGUI();
	}
	else {
		if (ImGui::Button("Enter debug")) {
			m_debug = true;
			g_gaWorld.suspend();
		}
	}
	ImGui::End();
}

Debugger::Debug::~Debug()
{
}
