#include "dfBSound.h"

#include "../dfVOC.h"

DarkForces::Behavior::Sound::Sound(const char* name) :
	GameEngine::Behavior::Sound(name)
{
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
