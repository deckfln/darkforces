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

	ImGui::Begin("Menu");                          // Create a window called "Hello, world!" and append into it.

	if (m_debug) {
		if (ImGui::Button("Exit debug")) {
			m_debug = false;
			m_requestDebug = false;
		}
		else {
			m_app->m_scene->debugGUI();
		}
	}
	else {
		if (ImGui::Button("Enter debug") || m_requestDebug) {
			m_debug = true;
			m_requestDebug = false;
		}
	}
	ImGui::End();
}

Framework::Debug::~Debug()
{
}
