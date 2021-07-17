#include "Debug.h"

#include <iostream>
#include <fstream>

#include "../myDarkForces.h"
#include "../gaEngine/gaActor.h"
#include "../gaEngine/World.h"

#include "../flightRecorder/Blackbox.h"

Debugger::Debug g_Debugger;

/**
 *
 */
Debugger::Debug::Debug()
{
}

/**
* load the flight recorder v1
*/
void Debugger::Debug::loadRecorderV1(void)
{
	std::ifstream myfile;
	myfile.open("player.txt", std::ios::in | std::ios::binary);
	myfile.read((char*)&m_recorder_len, sizeof(m_recorder_len));
	for (int i = 0; i < m_recorder_len; i++)
	{
		myfile.read((char*)&m_recorder[i], sizeof(m_recorder[0]));
	}
	myfile.close();
	m_recorder_end = 0;
}

/**
 * Play the flight recorder V1
 */
void Debugger::Debug::playRecorderV1(void)
{
	if (m_replay) {
		gaEntity* player = g_gaWorld.getEntity("player");
		player->sendInternalMessage(gaMessage::CONTROLLER, 0, &m_recorder[m_recorder_end++]);
		if (m_recorder_end >= m_recorder_len) {
			m_replay = false;
		}
	}
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

		ImGui::Text("flightRecorder");
		ImGui::SameLine(); if (ImGui::Button("Load")) {
			loadRecorderV1();
		}
		ImGui::SameLine(); if (ImGui::Button("Save")) {
			g_Blackbox.saveStates();
		}
		if (m_recorder_len > 0) {
			ImGui::SameLine(); if (ImGui::Button(">")) {
				if (!m_replay) {
					m_replay = true;
				}
				m_debug = false;
				g_gaWorld.run();
			}

			ImGui::SameLine(); ImGui::SliderInt("frame", &m_recorder_end, 0, m_recorder_len);
		}

		// display entities
		g_gaWorld.renderGUI();
	}
	else {
		if (m_replay) {
			playRecorderV1();
			ImGui::Text("flightRecorder");
			ImGui::SameLine(); if (ImGui::Button("||")) {
				m_debug = true;
				g_gaWorld.suspend();
			}
			ImGui::SameLine(); ImGui::SliderInt("frame", &m_recorder_end, 0, m_recorder_len);;
		}
		else {
			if (ImGui::Button("Enter debug")) {
				m_debug = true;
				g_gaWorld.suspend();
			}
		}
	}
	ImGui::End();
}

Debugger::Debug::~Debug()
{
}
