#include "gaBehaviorSound.h"

#include "../gaEntity.h"

#include <imgui.h>
#include <tinyxml2.h>

GameEngine::Behavior::Sound::Sound(const char *name) :
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sound::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Sound* node;;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Sound(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Sound*>(used);
	}

	const char* file;
	int32_t id;

	tinyxml2::XMLElement* sounds = element->FirstChildElement("sounds");
	if (sounds) {
		tinyxml2::XMLElement* sound = sounds->FirstChildElement("sound");

		while (sound != nullptr) {
			file = sound->Attribute("file");
			sound->QueryIntAttribute("id", &id);

			node->addSound(file, id);

			sound = sound->NextSiblingElement("sound");
		}
	}

	return node;
}

void GameEngine::Behavior::Sound::execute(Action* r)
{
	// only play a sound every maxdelay s
	if (time(nullptr) - m_delay < m_maxdelay) {
		return succeeded(r);
	}

	// lazy loading
	if (!m_loaded[m_currentSound]) {
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, m_ids[m_currentSound], m_sounds[m_currentSound]);
		m_loaded[m_currentSound] = true;
	}

	m_entity->sendMessage(gaMessage::Action::PLAY_SOUND, m_ids[m_currentSound]);
	m_delay = time(nullptr);

	// we are in the default loop
	switch (m_condition) {
	case Condition::IN_ORDER:
		m_currentSound++;
		if (m_currentSound >= m_sounds.size()) {
			m_currentSound = 0;
		}
		break;

	case Condition::RANDOM:
		m_currentSound = rand() % (m_sounds.size() - 1);
		break;
	}

	succeeded(r);
}

/**
 * register a new sound
 */
void GameEngine::Behavior::Sound::addSound(alSound* sound, uint32_t id)
{
	m_sounds.push_back(sound);
	m_ids.push_back(id);
	m_loaded.push_back(false);
}

void GameEngine::Behavior::Sound::addSound(const char* file, uint32_t id)
{
}

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::Sound::debugGUInode(void)
{
	switch (m_condition) {
	case Condition::IN_ORDER:
		ImGui::Text("Play in order");
		break;

	case Condition::RANDOM:
		ImGui::Text("Play randomly");
		break;
	}

	ImGui::Text("# sounds: %d", m_sounds.size());
}
