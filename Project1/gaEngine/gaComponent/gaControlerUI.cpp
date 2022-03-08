#include "gaControlerUI.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "../Debug.h"
#include "../gaMessage.h"
#include "../gaEntity.h"


GameEngine::Component::ControllerUI::ControllerUI(void) :
	gaComponent(gaComponent::Controller),
	fwControl(nullptr)
{
}

GameEngine::Component::ControllerUI::ControllerUI(const std::vector<KeyInfo>& keys):
	gaComponent(gaComponent::Controller),
	fwControl(nullptr),
	m_keys(keys)
{
}

/**
 * deal with movement keys
 */
bool GameEngine::Component::ControllerUI::checkKeys(time_t delta)
{
	// in recording mode, save the status
	for (auto& key : m_keys) {
		switch (key.m_msh) {
		case KeyInfo::Msg::onPressDown:
			if (m_currentKeys[key.m_key] && !m_prevKeys[key.m_key]) {
				m_entity->sendMessage(gaMessage::Action::KEY, key.m_key);
			}
			break;
		case KeyInfo::Msg::onPress:
			if (m_currentKeys[key.m_key]) {
				if (!m_prevKeys[key.m_key]) {
					m_entity->sendMessage(gaMessage::Action::KEY_DOWN, key.m_key);
				}
				else {
					m_entity->sendMessage(gaMessage::Action::KEY, key.m_key);
				}
			}
			if (!m_currentKeys[key.m_key] && m_prevKeys[key.m_key]) {
				m_entity->sendMessage(gaMessage::Action::KEY_UP, key.m_key);
			}
			break;
		case KeyInfo::Msg::onPressUp:
			if (!m_currentKeys[key.m_key] && m_prevKeys[key.m_key]) {
				m_entity->sendMessage(gaMessage::Action::KEY, key.m_key);
			}
			break;
		}
	}

	return true;
}

/**
 * Debugger
 */
void GameEngine::Component::ControllerUI::debugGUIinline(void)
{
	if (ImGui::TreeNode("Controller UI")) {
		ImGui::TreePop();
	}
}

