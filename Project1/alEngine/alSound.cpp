#include "alSound.h"

alSound::alSound(void)
{
	alGenBuffers((ALuint)1, &m_buffer);
}

void alSound::data(ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	alBufferData(m_buffer, format, data, size, freq);
}

alSound::~alSound()
{
	alDeleteBuffers((ALuint)1, &m_buffer);
}
