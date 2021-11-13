#include "alSource.h"

#include <AL/alc.h>
#include <iostream>

#include "alListener.h"
#include "alSound.h"

alSource::alSource(void)
{
}

/**
 * bind the given buffer and play it
 */
bool alSource::play(alSound* buffer, const glm::vec3& position)
{
	// drop the sound if it is too far from the listener
	if (g_Listener.clamp(position)) {
		return false;
	}

	ALint id = buffer->id();

	// clean running sources
	ALint state;
	ALint player = -1;
	for (auto& source : m_sources) {
		alGetSourcei(source.first, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED) {
			m_sources[source.first] = -1;
			player = source.first;
			break;
		}
	}

	// if none are available spawn a new one at the same location
	if (player < 0) {
		alGenSources((ALuint)1, (ALuint *)&player);

		// set attenuation
		alSourcef((ALuint)player, AL_ROLLOFF_FACTOR, 1.0f);
		alSourcef((ALuint)player, AL_REFERENCE_DISTANCE, g_Listener.mindistance());
		alSourcef((ALuint)player, AL_MAX_DISTANCE, g_Listener.maxdistance());

		m_sources[player] = -1;				// first player, no sound
	}

	// set position
	alSource3f((ALuint)player, AL_POSITION, position.x, position.y, position.z);

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

	return true;	// trigger the sound
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

