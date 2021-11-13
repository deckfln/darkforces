#include "alListener.h"

#include <glm/glm.hpp>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

alListener g_Listener;

static ALfloat g_ListenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };;
static ALCdevice* g_Device = nullptr;
static ALCcontext* g_Context = nullptr;

alListener::alListener()
{
	g_Device = alcOpenDevice(NULL);
	if (!g_Device) {
		std::cerr << "alListener::orientation error" << std::endl;
	}

	g_Context = alcCreateContext(g_Device, NULL);
	if (!alcMakeContextCurrent(g_Context)) {
		std::cerr << "alListener::orientation error" << std::endl;
	}
}

/**
 * set attenuation model
 */
void alListener::maxdistance(float m)
{
	alDistanceModel(AL_LINEAR_DISTANCE);
	m_maxdistance = m;
}

void alListener::position(const glm::vec3& position)
{
	m_position = position;
	alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void alListener::orientation(const glm::vec3& at, const glm::vec3& up)
{
	g_ListenerOri[0] = at.x; 	g_ListenerOri[1] = at.y; 	g_ListenerOri[2] = at.z;
	g_ListenerOri[3] = up.x; 	g_ListenerOri[4] = up.y; 	g_ListenerOri[3] = up.z;

	ALCenum error = alGetError();

	alListenerfv(AL_ORIENTATION, g_ListenerOri);
	error = alGetError();
	if (error != AL_NO_ERROR) {
		std::cerr << "alListener::orientation error" << std::endl;
	}
}

/**
 * sound gets clamped after the max distance
 */
bool alListener::clamp(const glm::vec3& p)
{
	return glm::distance(m_position, p) > m_maxdistance;
}

alListener::~alListener()
{
}
