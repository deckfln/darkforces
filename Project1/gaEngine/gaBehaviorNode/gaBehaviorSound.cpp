#include "gaBehaviorSound.h"

#include "../gaEntity.h"

#include <imgui.h>
#include <tinyxml2.h>

static std::map<const char*, GameEngine::Behavior::Sound::Condition> g_conditions = {
	{"play_in_order", GameEngine::Behavior::Sound::Condition::IN_ORDER},
	{"play_random", GameEngine::Behavior::Sound::Condition::RANDOM},
};

static const char* g_className = "Sound";

GameEngine::Behavior::Sound::Sound(const char *name) :
	BehaviorNode(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sound::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Sound* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Sound*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Sound(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);

	cl->m_condition = m_condition;
	for (auto& sound : m_sounds) {
		cl->m_sounds.push_back(sound);
		cl->m_loaded.push_back(false);

	}
	for (auto& id: m_ids) {
		cl->m_ids.push_back(id);
	}
	return cl;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sound::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Sound* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Sound(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Sound*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	tinyxml2::XMLElement* attr = element->FirstChildElement("condition");
	if (attr) {
		const char* t = attr->GetText();
		for (auto& c : g_conditions) {
			if (strcmp(t, c.first) == 0) {
				node->m_condition = c.second;
				break;
			}
		}
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

/**
 * 
 */
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

//-----------------------------------------------

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::Sound::debugGUInode(void)
{
	static char tmp[64];
	switch (m_condition) {
	case Condition::IN_ORDER:
		sprintf_s(tmp, "Play in order %d", m_currentSound);
		break;

	case Condition::RANDOM:
		strcpy_s(tmp, "Play randomly");
		break;
	}

	if (ImGui::TreeNode(tmp)) {
		for (auto i = 0; i < m_sounds.size(); i++) {
			ImGui::Text("%d: %s", m_ids[i], m_sounds[i]->source().c_str());
		}
		ImGui::TreePop();
	}
}
