#pragma once

#include <AL/al.h>
#include <glm/vec3.hpp>

class alSound;

class alSource
{
	ALuint m_source = 0;
	ALint m_state = 0;

public:
	alSource(void);
	void position(glm::vec3& position);
	void play(alSound* buffer);
	bool play(void);
	void stop(void);
	~alSource();
};