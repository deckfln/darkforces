#include "gaController.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "../Debug.h"
#include "../gaMessage.h"
#include "../gaEntity.h"

GameEngine::Component::Controller::Controller(fwCamera* camera,
	const glm::vec3& position,
	float height,
	float phi,
	float radius,
	const std::vector<uint32_t>& keys):
	gaComponent(gaComponent::Controller),
	fwControlThirdPerson(camera, position, height, phi, radius),
	m_keys(keys)
{
}

GameEngine::Component::Controller::~Controller()
{
}

void GameEngine::Component::Controller::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE:
		m_position = m_entity->position();
		break;
	}
}

void GameEngine::Component::Controller::debugGUIinline(void)
{
	if (ImGui::TreeNode("Controller")) {
		ImGui::TreePop();
	}
}

/**
 * deal with movement keys
 */
bool GameEngine::Component::Controller::checkKeys(time_t delta)
{
	fwControlThirdPerson::checkKeys(delta);

	// in recording mode, save the status
#ifdef DEBUG0
	//TODO find how to pass the information to the debugger
	if (m_currentKeys[GLFW_KEY_F1]) {
		g_Debugger.debugMode(true);
	}
#endif

	for (auto key : m_keys) {
		if (m_currentKeys[key] && !m_prevKeys[key]) {
			m_entity->sendInternalMessage(gaMessage::Action::KEY, key);
		}
	}

	return true;
}

void GameEngine::Component::Controller::updatePlayer(time_t delta)
{
	if (m_velocity != glm::vec3(0)) {
		m_entity->sendInternalMessage(gaMessage::CONTROLLER, (uint32_t)delta, &m_velocity);
	}

	if (m_eye != m_oldEye || m_lookAt != m_oldLookAt) {
		m_entity->sendInternalMessage(gaMessage::Action::LOOK_AT, 0, &m_lookDirection);
		m_entity->sendInternalMessage(gaMessage::Action::MOVE_AT, 0, &m_eye);

		m_oldEye = m_eye;
		m_oldLookAt = m_lookAt;
	}
}