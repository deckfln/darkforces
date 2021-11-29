#pragma once

#include <AL/al.h>
#include <string>

class alSound {
	ALuint m_buffer = 0;
	bool m_repeat = false;
	std::string m_source;		// source file name

public:
	alSound(void);
	alSound(ALenum format, ALvoid* data, ALsizei size, ALsizei freq, bool repeat);
	inline ALuint id(void) { return m_buffer; };
	inline bool repeat(void) { return m_repeat; };
	inline void source(const std::string& source) { m_source = source; };
	inline const std::string& source(void) { return m_source; };
	void data(ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
	~alSound();
};