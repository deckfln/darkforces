#include "Debug.h"

#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>

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

	/*
	// Create the docking environment
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");

	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
	*/
	// create defaul layout	
	ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
	bool dockSpaceCreated = ImGui::DockBuilderGetNode(dockSpaceId) != nullptr;
	if (!dockSpaceCreated) {
		ImGui::DockBuilderRemoveNode(dockSpaceId);
		ImGuiDockNodeFlags dockSpaceFlags = 0;
		dockSpaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockBuilderAddNode(dockSpaceId, dockSpaceFlags);

		ImGuiID dockMain = dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.40f, NULL, &dockMain);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.40f, NULL, &dockMain);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.40f, NULL, &dockMain);

		ImGui::DockBuilderDockWindow("Player", dockLeft);
		ImGui::DockBuilderDockWindow("Entities", dockLeft);
		ImGui::DockBuilderDockWindow("Monitor", dockRight);
		ImGui::DockBuilderFinish(dockSpaceId);
	}

	ImGui::Begin("Player");                          // Create a window called "Hello, world!" and append into it.

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

		if (ImGui::Button("Exit debug")) {
			m_debug = false;
			m_requestDebug = false;
		}
		ImGui::End();

		g_Blackbox.debugGUI();
		ImGui::Begin("FlightRecorder v1");
			if (ImGui::Button("Load##1")) {
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
		ImGui::End();

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
		ImGui::End();
	}

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
