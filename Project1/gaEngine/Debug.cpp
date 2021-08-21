#include "Debug.h"

#include <iostream>
#include <fstream>
#include <imgui.h>
#include <imgui_internal.h>

#include "../gaEngine/gaActor.h"
#include "../gaEngine/World.h"

#include "../myDarkForces.h"
#include "../flightRecorder/Blackbox.h"

/**
 *
 */
GameEngine::Debug::Debug(myDarkForces *app):
	Framework::Debug::Debug((fwApp *)app)
{
	m_control = new fwOrbitControl(nullptr, 20, glm::vec3(0));
}

/**
* load the flight recorder v1
*/
void GameEngine::Debug::loadRecorderV1(void)
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
void GameEngine::Debug::playRecorderV1(void)
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
 * Render the interface
 */
void GameEngine::Debug::render(void)
{
	bool old_state = m_debug;
	myDarkForces* app = static_cast<myDarkForces *>(m_app);

	// create defaul layout	
	/*
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
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags); // This is basically the background window that contains all the dockable windows
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");
	if (!ImGui::DockBuilderGetNode(dockSpaceId)) {
		ImGui::DockBuilderRemoveNode(dockSpaceId);
		ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;;
		dockSpaceFlags |= ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace);

		ImGuiID dockMain = dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.40f, NULL, &dockMain);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Right, 0.40f, NULL, &dockMain);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Down, 0.40f, NULL, &dockMain);

		ImGui::DockBuilderDockWindow("MainWindow", dockMain);
		ImGui::DockBuilderDockWindow("Player", dockBottom);
		ImGui::DockBuilderDockWindow("Explorer", dockLeft);
		ImGui::DockBuilderDockWindow("Inspector", dockRight);
		ImGui::DockBuilderDockWindow("Messages", dockRight);
		ImGui::DockBuilderFinish(dockSpaceId);
	}
	*/
	Framework::Debug::render();

	/*
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("xView")) {
			if (ImGui::MenuItem("xExplorer")) {
				//Do something
			}
			if (ImGui::MenuItem("xInspector")) {
				//Do something
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	*/
	bool b = true;
	if (ImGui::Begin("Menu", &b)) {
		glm::vec3 p = app->m_player->position();
		ImGui::Text("Player x:%.3f y:%.3f z:%.3f", p.x, p.y, p.z);

		int messages = g_gaWorld.queueLen();
		ImGui::Text("Messages: %d", messages);
		ImGui::Text("Frame: %d", g_gaWorld.frame());

		// monitor the queue len and trigger the debugger if needed
		if (messages > 1024) {
			m_debug = true;
		}
		ImGui::End();
	}

	if (m_debug || m_framebyframe) {
		if (m_framebyframe) {
			g_gaWorld.suspend();
		}

		if (m_control->isKeyPressed(GLFW_KEY_RIGHT)) {
			g_Blackbox.nextFrame();
		}
		if (m_control->isKeyPressed(GLFW_KEY_LEFT)) {
			g_Blackbox.previousFrame();
		}

		g_Blackbox.debugGUI();
		/*
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
		*/
		// display all entities to pick from
		g_gaWorld.debugGUI();
		g_Blackbox.debugGUIinframe();
	}
	else {
		if (m_replay) {
			playRecorderV1();
			ImGui::Begin("FlightRecorder v1");
			ImGui::SameLine(); if (ImGui::Button("||")) {
				m_debug = true;
				g_gaWorld.suspend();
			}
			ImGui::SameLine(); ImGui::SliderInt("frame", &m_recorder_end, 0, m_recorder_len);;
			ImGui::End();
		}
	}

	// if we are entering debug state, 
	// save the game camera & control
	// and inject the debugger camera & control
	if (old_state == false && m_debug == true) {
		app->m_camera->push();
		m_gameControl = (fwControl *)app->m_control;
		m_control->bindCamera(app->m_camera);
		m_control->setFromCamera();
		m_app->bindControl((fwControl*)m_control);
		g_gaWorld.suspend();
	}

	// if we leave debug state, 
	// restore the game camera & control
	if (old_state == true && m_debug == false) {
		app->m_camera->pop();
		app->bindControl(m_gameControl);
		m_gameControl = nullptr;
		g_gaWorld.run();
	}
}

GameEngine::Debug::~Debug()
{
}
