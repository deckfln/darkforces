#pragma once

#include <AL/al.h>

class alSound {
	ALuint m_buffer = 0;
	bool m_repeat = false;

public:
	alSound(void);
	alSound(ALenum format, ALvoid* data, ALsizei size, ALsizei freq, bool repeat);
	ALuint id(void) { return m_buffer; };
	bool repeat(void) { return m_repeat; };
	void data(ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
	~alSound();
};