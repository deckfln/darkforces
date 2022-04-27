#include "gaListener.h"

#include <imgui.h>

static const char* g_componentName = "Listener";

alSource GameEngine::Component::Listener::m_source;

void GameEngine::Component::Listener::onHearSound(gaMessage* message)
{
	// Start playing a sound or check if it plays
	alSound* sound = static_cast<alSound*>(message->m_extra);
	if (sound) {
		glm::vec3 p = message->m_v3value;
		m_source.play(sound, p);
	}
}

void GameEngine::Component::Listener::onHearStop(gaMessage* message)
{
	// Stop playing a sound (or all sound if nullptr)
	alSound* sound = static_cast<alSound*>(message->m_extra);
	if (sound) {
		m_source.stop(sound);
	}
}

GameEngine::Component::Listener::Listener(void):
	gaComponent(gaComponent::Listener)
{
}

/**
 * let a component deal with a situation
 */
void GameEngine::Component::Listener::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::HEAR_SOUND:
		onHearSound(message);
		break;

	case gaMessage::HEAR_STOP:
		onHearStop(message);
		break;
	}
}

#ifdef _DEBUG
/**
 * display the component in the debugger
 */
void GameEngine::Component::Listener::debugGUIinline(void)
{
	if (ImGui::TreeNode(g_componentName)) {
		ImGui::TreePop();
	}
}
#endif