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

			if (message->m_value == 1024) {
				gaEntity* player = g_gaWorld.getEntity("player");

				std::vector<GameEngine::Sound::Virtual> virtualSources;
				g_gaLevel->volume().path(p, player->position(), 50.0f, virtualSources);
			}
			m_source.play(sound, p);
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

void GameEngine::Component::Sound::position(const glm::vec3& position)
{
	m_position = position;
}

GameEngine::Component::Sound::~Sound()
{
}

