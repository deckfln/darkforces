#include "fwDebug.h"

#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>

#include "../myDarkForces.h"
#include "../gaEngine/gaActor.h"
#include "../gaEngine/World.h"

#include "../flightRecorder/Blackbox.h"
#include "fwApp.h"
#include "fwScene.h"

/**
 *
 */
Framework::Debug::Debug(fwApp* app):
	m_app(app)
{
}

/**
 * Change debug state
 */
void Framework::Debug::debugMode(bool mode)
{
	m_requestDebug = mode;
}

/**
 * Render the interface
 */
void Framework::Debug::render(void)
{
	bool old_state = m_debug;

	ImGui::Begin("flightRecorder v2");                          // Create a window called "Hello, world!" and append into it.

	if (m_debug) {
		if (ImGui::Button(">")) {
			m_debug = false;
			m_requestDebug = false;
		}
	}
	else {
		if (ImGui::Button("||") || m_requestDebug) {
			m_debug = true;
			m_requestDebug = false;
		}
	}
	ImGui::SameLine();
	ImGui::End();
	m_app->m_scene->debugGUI(m_debug);
}

Framework::Debug::~Debug()
{
}
