#include "gaPSounds.h"

#include "../../alEngine/alSound.h"

#include "../gaEntity.h"
#include "../gaComponent/gaAIPerception.h"
#include "../gaVolumeSpace.h"
#include "../gaLevel.h"

GameEngine::Plugins::Sounds g_gaSoundEngine;

void GameEngine::Plugins::Sounds::onPropagateSound(gaEntity* from, gaMessage* message)
{
	const uint32_t soundID = message->m_value;
	const glm::vec3& p = message->m_v3value;
	alSound* sound = static_cast<alSound*>(message->m_extra);

	gaEntity* entity;
	Component::AIPerception* perception;
	std::vector<GameEngine::Sound::Virtual> virtualSources;

	// check audio perceptions
	for (auto& pair : m_hear) {
		entity = pair.second;
		perception = dynamic_cast<Component::AIPerception*>(entity->findComponent(gaComponent::AIPerception));

		// if the entity register the sounds it wants to hear
		if (perception) {
			const std::vector<uint32_t> sounds = perception->heardSound();

			// if there are sound registered, only run the process for these sounds
			if (sounds.size() > 0) {
				bool process = false;
				for (auto s : sounds) {
					if (soundID == s) {
						process = true;
						break;
					}
				}
				if (!process) {
					continue;
				}
			}
		}

		virtualSources.clear();
		g_gaLevel->volume().path(p, entity->position(), 50.0f, virtualSources);

		// ask the player to play the sound
		if (virtualSources.size() > 0) {
			for (auto& vs : virtualSources) {
				from->sendMessage(entity->name(), gaMessage::Action::HEAR_SOUND, soundID, vs.loundness, vs.origin, sound);
			}
		}
	}
}

void GameEngine::Plugins::Sounds::onChangeVolumeTransparency(gaEntity* to, gaMessage* message)
{
	g_gaLevel->volume().transparency(message->m_value, message->m_fvalue);
}

/**
 *
 */
GameEngine::Plugins::Sounds::Sounds(void):
	GameEngine::Plugin("Sounds")
{
}

/**
 * (de)register entities wanting to hear sounds
 */
void GameEngine::Plugins::Sounds::registerHearEvents(gaEntity* entity)
{
	uint32_t id = entity->entityID();
	if (m_hear.count(id) == 0) {
		m_hear[id] = entity;
	}
}

void GameEngine::Plugins::Sounds::deRegisterHearEvents(gaEntity* entity)
{
	uint32_t id = entity->entityID();
	if (m_hear.count(id) != 0) {
		m_hear.erase(id);
	}
}

/**
 * 
 */
bool GameEngine::Plugins::Sounds::dispatchMessage(gaEntity*to, gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::PROPAGATE_SOUND:
		onPropagateSound(to, message);
		break;

	case gaMessage::Action::VOLUME_TRANSPARENCY:
		onChangeVolumeTransparency(to, message);
		return false;	// intercept the message
	}
	return true;
}
