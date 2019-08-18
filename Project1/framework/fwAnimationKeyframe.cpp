#include "fwAnimationKeyframe.h"

#include <iostream>

fwAnimationKeyframe::fwAnimationKeyframe(time_t time) :
	m_time(time)
{
}

void fwAnimationKeyframe::interpolate(fwAnimationKeyframe* next, float delta, glm::mat4* target)
{

}

fwAnimationKeyframe::~fwAnimationKeyframe()
{
}
