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
GameEngine::Debug::Debug(App *app):
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
	Framework::Debug::render();

	bool b = true;
	if (ImGui::Begin("Menu")) {
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

	// display the flight recorder
	bool loadedFrame = g_Blackbox.debugGUI(m_debug);

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

		// and update the world
		if (loadedFrame) {
			g_gaWorld.process(0, true, true);	// force execution in debug mode
			g_gaWorld.update();
		}
	}
	else {
		if (m_replay) {
			/*
			/playRecorderV1();
			ImGui::Begin("FlightRecorder v1");
			ImGui::SameLine(); if (ImGui::Button("||")) {
				m_debug = true;
				g_gaWorld.suspend();
			}
			ImGui::SameLine(); ImGui::SliderInt("frame", &m_recorder_end, 0, m_recorder_len);;
			ImGui::End();
			*/
		}
	}

	// display all elements currently on the queue
	g_gaWorld.debugGUI(m_debug);
	g_Blackbox.debugGUImessages(m_debug);
	g_Blackbox.debugGUIinframe(m_debug);

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
