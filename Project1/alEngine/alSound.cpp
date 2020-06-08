#include "alSound.h"

#include <AL/alc.h>
#include <iostream>

alSound::alSound(void)
{
	alGenBuffers((ALuint)1, &m_buffer);
}

alSound::alSound(ALenum format, ALvoid* data, ALsizei size, ALsizei freq, bool repeat): 
	m_repeat(repeat)
{
	ALCenum error = alGetError();

	alGenBuffers((ALuint)1, &m_buffer);
	alBufferData(m_buffer, format, data, size, freq);

	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "alSound::alSound error" << std::endl;
	}
}

void alSound::data(ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	alBufferData(m_buffer, format, data, size, freq);
}

alSound::~alSound()
{
	alDeleteBuffers((ALuint)1, &m_buffer);
}
