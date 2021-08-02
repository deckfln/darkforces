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
	m_control = new fwOrbitControl(nullptr, 20, glm::vec3(0));
}

/**
* load the flight recorder v1
*/
void Debugger::Debug::loadRecorderV1(void)
{
	std::ifstream myfile;
	myfile.open("D:/dev/Project1/Project1/player.txt", std::ios::in | std::ios::binary);
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
	m_requestDebug = mode;
}

/**
 * Render the interface
 */
void Debugger::Debug::render(myDarkForces *dark)
{
	bool old_state = m_debug;

	ImGui::Begin("Debugger");                          // Create a window called "Hello, world!" and append into it.

	glm::vec3 p = dark->m_player->position();
	ImGui::Text("Player x:%.3f y:%.3f z:%.3f", p.x, p.y, p.z);

	p = dark->m_camera->position();
	glm::vec3 p1 = dark->m_camera->lookAt();
	ImGui::Text("camera x:%.3f y:%.3f z:%.3f->x:%.3f y:%.3f z:%.3f", p.x, p.y, p.z, p1.x, p1.y, p1.z);

	// display entities

	if (m_debug || m_framebyframe) {
		if (m_framebyframe) {
			g_gaWorld.suspend();
		}

		ImGui::BeginGroup();
		if (ImGui::Button("Exit debug")) {
			m_debug = false;
			m_requestDebug = false;
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
			ImGui::BeginGroup();
				ImGui::Text("flightRecorder v2");
				ImGui::SameLine(); if (ImGui::Button("Load")) {
					g_Blackbox.loadStates();
					g_Blackbox.setState(m_frame);
				}
				ImGui::SameLine(); if (ImGui::Button("Save")) {
					g_Blackbox.saveStates();
				}
				if (g_Blackbox.len() > 0) {
					ImGui::SameLine(); if (ImGui::Button(">")) {
						if (!m_replay) {
							m_replay = true;
						}
						m_debug = false;
						g_gaWorld.run();
					}

					int old_frame = m_frame;
					ImGui::SameLine(); ImGui::SliderInt("frame", &m_frame, 0, g_Blackbox.len());
					if (old_frame != m_frame) {
						// Set the game state
						g_Blackbox.setState(m_frame);

						// set the debug camera based on the player position
						//m_control->translateCamera(
						//	dark->m_player->position()
						//	);
					}

					if (m_frame < g_Blackbox.len()) {
						// display frame by frame up to the last frame
						ImGui::SameLine(); if (ImGui::Button(">>")) {
							g_gaWorld.run();

							m_framebyframe = true;
							m_replay = true;
							m_debug = false;
						}
					}
				}
			ImGui::EndGroup();

			ImGui::BeginGroup();
				ImGui::Text("flightRecorder v1");
				ImGui::SameLine(); if (ImGui::Button("Load##1")) {
					loadRecorderV1();
				}
				if (m_recorder_len > 0) {
					ImGui::SameLine(); if (ImGui::Button(">##1")) {
						if (!m_replay) {
							m_replay = true;
						}
						m_debug = false;
						g_gaWorld.run();
					}

					ImGui::SameLine(); ImGui::SliderInt("f##1", &m_recorder_end, 0, m_recorder_len);
					if (m_recorder_end < m_recorder_len) {
						// display frame by frame up to the last frame
						ImGui::SameLine(); if (ImGui::Button(">>##1")) {
							playRecorderV1();
							g_gaWorld.run();

							m_framebyframe = true;
							m_replay = true;
							m_debug = false;
						}
					}
				}
			ImGui::EndGroup();

		ImGui::EndGroup();

		// display all entities to pick from
		g_gaWorld.debugGUI();
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
			if (ImGui::Button("Enter debug") || m_requestDebug) {
				m_debug = true;
				m_requestDebug = false;
			}
		}
	}

	ImGui::End();

	// if we are entering debug state, 
	// save the game camera & control
	// and inject the debugger camera & control
	if (old_state == false && m_debug == true) {
		dark->m_camera->push();
		m_gameControl = (fwControl *)dark->m_control;
		m_control->bindCamera(dark->m_camera);
		m_control->setFromCamera();
		dark->bindControl((fwControl*)m_control);
		g_gaWorld.suspend();
	}

	// if we leave debug state, 
	// restore the game camera & control
	if (old_state == true && m_debug == false) {
		dark->m_camera->pop();
		dark->bindControl(m_gameControl);
		m_gameControl = nullptr;
		g_gaWorld.run();
	}
}

Debugger::Debug::~Debug()
{
}
