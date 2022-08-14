#include "gaListener.h"

#include <imgui.h>

static const char* g_componentName = "Listener";

alSource GameEngine::Component::Listener::m_source;

/**
 * sound in a list of sound
 */
void GameEngine::Component::Listener::onHearSoundNext(gaMessage* message)
{
	// only keep the sound if it is louder that the previous we received
	if (message->m_fvalue > m_source_loundness) {
		m_source_loundness = message->m_fvalue;
		m_source_position = message->m_v3value;
	}
}

/**
 * last sound of the batch
 */
void GameEngine::Component::Listener::onHearSound(gaMessage* message)
{
	// Start playing a sound or check if it plays
	if (message->m_fvalue > m_source_loundness) {
		m_source_loundness = message->m_fvalue;
		m_source_position = message->m_v3value;
	}

	alSound* sound = static_cast<alSound*>(message->m_extra);
	if (sound) {
		m_source.play(sound, m_source_position);
	}
	m_source_loundness = -1.0f;
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
	case gaMessage::HEAR_SOUND_NEXT:
		onHearSoundNext(message);
		break;

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