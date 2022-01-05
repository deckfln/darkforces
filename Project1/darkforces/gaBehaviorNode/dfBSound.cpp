#include "dfBSound.h"

#include "../dfVOC.h"

static const char* g_className = "DarkForces:Sound";

DarkForces::Behavior::Sound::Sound(const char* name) :
	GameEngine::Behavior::Sound(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* DarkForces::Behavior::Sound::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::Sound* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::Sound*>(p);
	}
	else {
		cl = new DarkForces::Behavior::Sound(m_name);
	}
	GameEngine::Behavior::Sound::clone(cl);
	return cl;
}

GameEngine::BehaviorNode* DarkForces::Behavior::Sound::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Sound* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::Sound(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::Sound*>(used);
	}

	return GameEngine::Behavior::Sound::create(name, element, node);
}

/**
 * register a new sound
 */

void DarkForces::Behavior::Sound::addSound(const char* file, uint32_t id)
{
	GameEngine::Behavior::Sound::addSound(loadVOC(file)->sound(), id);
}
