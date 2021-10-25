#include "gaSound.h"

#include "../gaEntity.h"

#include <imgui.h>

/**
 *
 */
GameEngine::Component::Sound::Sound(void):
	gaComponent(gaComponent::SOUND),
	m_source(m_position)
{
}

/**
 * let a component deal with a situation
 */
void GameEngine::Component::Sound::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE: {
		if (message->m_extra != nullptr) {
			position(*(glm::vec3*)message->m_extra);
		}
		else {
			position(m_entity->position());
		}
		break;
	}

	case gaMessage::Action::REGISTER_SOUND:
		addSound(message->m_value, static_cast<alSound*>(message->m_extra));
		break;

	case gaMessage::PLAY_SOUND: {
		// Start playing a sound or check if it plays
		alSound* sound = m_sounds[message->m_value];
		if (sound) {
			m_source.play(sound);
		}
		break;
	}
	case gaMessage::STOP_SOUND: {
		// Stop playing a sound (or all sound if nullptr)
		alSound* sound = m_sounds[message->m_value];
		if (sound) {
			m_source.stop(sound);
		}
		break;
	}
	}
}

/**
 * display the component in the debugger
 */
void GameEngine::Component::Sound::debugGUIinline(void)
{
	if (ImGui::TreeNode("Sound")) {
		ImGui::Text("position: %.2f %.2f %.2f", m_position.x, m_position.y, m_position.z);
		ImGui::TreePop();
	}
}

/**
 * register sounds by name
 */
void GameEngine::Component::Sound::addSound(uint32_t name, alSound* sound)
{
	m_sounds[name] = sound;
}

/**
 * define the position of the source
 */
void GameEngine::Component::Sound::position(const glm::vec3 &position)
{
	m_position = position;
	m_source.position(m_position);
}

GameEngine::Component::Sound::~Sound()
{
}

