#include "alSource.h"

#include <AL/alc.h>
#include <iostream>

#include "alSound.h"

alSource::alSource(void)
{
	alGenSources((ALuint)1, &m_source);
}

alSource::alSource(glm::vec3& position)
{
	alGenSources((ALuint)1, &m_source);
	alSource3f(m_source, AL_POSITION, position.x, position.y, position.z);
}

void alSource::position(glm::vec3& position)
{
	alSource3f(m_source, AL_POSITION, position.x, position.y, position.z);
}

/**
 * bind the given buffer and play it
 */
void alSource::play(alSound* buffer)
{
	ALCenum error = alGetError();

	alSourcei(m_source, AL_BUFFER, buffer->id());
	if (buffer->repeat()) {
		alSourcei(m_source, AL_LOOPING, AL_TRUE);
	}
	else {
		alSourcei(m_source, AL_LOOPING, AL_FALSE);
	}

	alSourcePlay(m_source);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "alSource::play error" << std::endl;
	}
}

/**
 * Is the source playing something ?
 */
bool alSource::play(void)
{
	alGetSourcei(m_source, AL_SOURCE_STATE, &m_state);
	return m_state == AL_PLAYING;
}

/**
 * stop the current play (if any)
 */
void alSource::stop(void)
{
	alSourceStop(m_source);
}

alSource::~alSource()
{
	alDeleteSources((ALuint)1, &m_source);
}

