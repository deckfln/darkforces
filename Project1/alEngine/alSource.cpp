#include "alSource.h"

#include <AL/alc.h>
#include <iostream>

#include "alSound.h"

alSource::alSource(void)
{
	ALuint source;

	alGenSources((ALuint)1, &source);
	m_sources[source] = -1;				// first player, no sound
}

alSource::alSource(const glm::vec3& position) :
	m_position(position)
{
	ALuint source;
	alGenSources((ALuint)1, &source);
	m_sources[source] = -1;				// first player, no sound

	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
}

void alSource::position(const glm::vec3& position)
{
	m_position = position;

	// move all player from the source
	for (auto& source : m_sources) {
		alSource3f(source.first, AL_POSITION, position.x, position.y, position.z);
	}
}

void alSource::position(const glm::vec3* pos)
{
	m_position = *pos;
	position(m_position);
}

/**
 * bind the given buffer and play it
 */
bool alSource::play(alSound* buffer)
{
	ALint id = buffer->id();

	// check if a player is playing that sound
	ALint state;
	for (auto& source : m_sources) {
		alGetSourcei(source.first, AL_SOURCE_STATE, &state);
		if (source.second == id) {
			if (m_state == AL_PLAYING) {
				return true;		// already playing that sound
			}
		}

		if (m_state == AL_STOPPED) {
			m_sources[source.first] = -1;
		}
	}

	// find an available player
	ALint player = -1;
	for (auto& source : m_sources) {
		if (source.second == -1) {
			player = source.first;
			break;
		}
	}

	// if none are available spawn a new one at the same location
	if (player < 0) {
		alGenSources((ALuint)1, (ALuint *)&player);
		m_sources[player] = -1;				// first player, no sound
		alSource3f((ALuint)player, AL_POSITION, m_position.x, m_position.y, m_position.z);
	}

	ALCenum error = alGetError();

	alSourcei(player, AL_BUFFER, id);
	if (buffer->repeat()) {
		alSourcei(player, AL_LOOPING, AL_TRUE);
	}
	else {
		alSourcei(player, AL_LOOPING, AL_FALSE);
	}

	alSourcePlay(player);
	m_sources[player] = id;

	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "alSource::play error" << std::endl;
	}

	return false;	// was not playing that 
}

/**
 * Is the source playing something ?
 */
bool alSource::play(void)
{
	ALint state;

	// move all player from the source
	for (auto& source : m_sources) {
		alGetSourcei(source.first, AL_SOURCE_STATE, &state);
		if (m_state == AL_PLAYING) {
			return true;
		}
	}
	return false;
}

/**
 * stop the current play (if any)
 */
void alSource::stop(alSound *sound)
{
	ALint id = -1;
	if (sound) {
		id = sound->id();
	}

	for (auto& source : m_sources) {
		if (id == -1 || id == source.second) {
			alSourceStop(source.first);
		}
	}
}

alSource::~alSource()
{
	for (auto& source : m_sources) {
		alDeleteSources((ALuint)1, (ALuint *)&source.second);
	}
}

