#pragma once

#include <AL/al.h>

class alSound {
	ALuint m_buffer = 0;
public:
	alSound(void);
	ALuint id(void) { return m_buffer; };
	void data(ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
	~alSound();
};