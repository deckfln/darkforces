#include "gaBPsound.h"

#include "../gaBehaviorNode/gaBSounds.h"

GameEngine::Behavior::Plugin::Sounds::Sounds(void):
	Base("GameEngine:Sounds")
{
}

/**
 * first message 'hear' a blaster, reset the list of sounds
 */
bool GameEngine::Behavior::Plugin::Sounds::onHearSoundFirst(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	// keep all sounds in list mode, messages are ALREADY sorted by volume
	GameEngine::Behavior::Sounds::Origins& sounds = blackboard.get<GameEngine::Behavior::Sounds::Origins>("sounds", GameEngine::Variable::Type::OBJECT);
	sounds.clear();
	sounds.push_back(message->m_v3value, message->m_fvalue);

	return true;
}

/**
 * partial message hear
 */
bool GameEngine::Behavior::Plugin::Sounds::onHearSoundNext(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	// keep all sounds in list mode, messages are ALREADY sorted by volume
	GameEngine::Behavior::Sounds::Origins& sounds = blackboard.get<GameEngine::Behavior::Sounds::Origins>("sounds", GameEngine::Variable::Type::OBJECT);
	sounds.push_back(message->m_v3value, message->m_fvalue);

	return true;
}

/**
 * partial message hear
 */
bool GameEngine::Behavior::Plugin::Sounds::onHearSoundLast(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	// sort the result
	GameEngine::Behavior::Sounds::Origins& sounds = blackboard.get<GameEngine::Behavior::Sounds::Origins>("sounds", GameEngine::Variable::Type::OBJECT);
	sounds.sort();

	// mark the sound heard only at the last message
	blackboard.set<bool>("heard_sound", true, GameEngine::Variable::Type::BOOL);

	return true;
}

void GameEngine::Behavior::Plugin::Sounds::dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::HEAR_SOUND_FIRST:
		onHearSoundFirst(blackboard, message);
		break;
	case gaMessage::HEAR_SOUND_NEXT:
		onHearSoundNext(blackboard, message);
		break;
	case gaMessage::HEAR_SOUND_LAST:
		onHearSoundLast(blackboard, message);
		break;
	}
}

GameEngine::Behavior::Plugin::Base* GameEngine::Behavior::Plugin::Sounds::Create(void)
{
	return new GameEngine::Behavior::Plugin::Sounds();
}
