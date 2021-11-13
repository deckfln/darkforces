#pragma once

#include <AL/al.h>
#include <glm/vec3.hpp>
#include <map>

class alSound;

class alSource
{
	std::map<ALuint, ALint> m_sources;	// dictionnary of sources playing buffers
	ALint m_state = 0;

public:
	alSource(void);
	bool play(alSound* buffer, const glm::vec3& position);	// play the sound (return false) or says it is already running(true)
	bool play(void);			// is playing something ?
	void stop(alSound* buffer = nullptr);
	~alSource();
};