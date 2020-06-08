#include "alSource.h"

#include "alSound.h"

alSource::alSource(void)
{
	alGenSources((ALuint)1, &m_source);
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
	alSourcei(m_source, AL_BUFFER, buffer->id());
	alSourcePlay(m_source);
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

