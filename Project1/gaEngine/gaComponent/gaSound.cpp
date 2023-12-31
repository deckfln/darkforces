#include "gaSound.h"

#include "../gaEntity.h"
#include "../gaLevel.h"
#include "../World.h"

#include <imgui.h>

alSource GameEngine::Component::Sound::m_source;

/**
 *
 */
GameEngine::Component::Sound::Sound(void):
	gaComponent(gaComponent::SOUND)
{
}

/**
 * let a component deal with a situation
 */
void GameEngine::Component::Sound::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::REGISTER_SOUND:
		addSound(message->m_value, static_cast<alSound*>(message->m_extra));
		break;

	case gaMessage::PLAY_SOUND: {
		// Start playing a sound or check if it plays
		alSound* sound = m_sounds[message->m_value];
		if (sound) {
			glm::vec3 p;
			if (message->m_v3value != glm::vec3(0)) {
				p = message->m_v3value;
			}
			else {
				p = m_entity->position();
			}
			m_entity->sendMessage(gaMessage::Action::PROPAGATE_SOUND, message->m_value, p, sound);
		}
		break;
	}
	case gaMessage::STOP_SOUND: {
		// Stop playing a sound (or all sound if nullptr)
		alSound* sound = m_sounds[message->m_value];
		m_entity->sendMessage("player", gaMessage::Action::HEAR_STOP, message->m_value, sound);
		break;
	}
	}
}

#ifdef _DEBUG
/**
 * display the component in the debugger
 */
void GameEngine::Component::Sound::debugGUIinline(void)
{
	if (ImGui::TreeNode("Sounds")) {
		for (auto& s : m_sounds) {
			ImGui::Text("%d", s.first);
		}
		ImGui::TreePop();
	}
}
#endif

/**
 * register sounds by name
 */
void GameEngine::Component::Sound::addSound(uint32_t name, alSound* sound)
{
	if (sound) {
		m_sounds[name] = sound;
	}
	else {
		m_sounds.erase(name);
	}
}

uint32_t GameEngine::Component::Sound::addSound(alSound* sound)
{
	uint32_t id = 0;

	// find the last index
	for (auto& sound : m_sounds) {
		if (id <= sound.first) {
			id = sound.first + 1;
		}
	}
	m_sounds[id] = sound;
	return id;
}

void GameEngine::Component::Sound::position(const glm::vec3& position)
{
	m_position = position;
}

GameEngine::Component::Sound::~Sound()
{
}

