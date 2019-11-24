#include "fwAnimationKeyframe.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

fwAnimationKeyframe::fwAnimationKeyframe(time_t time) :
	m_time(time)
{
}

glm::mat4 fwAnimationKeyframe::interpolate(fwAnimationKeyframe* next, float delta)
{
	glm::vec3 p = glm::mix(this->m_translation, next->m_translation, delta);
	glm::quat r = glm::slerp(this->m_rotation, next->m_rotation, delta);	// use slerp instead of mix to ensure 'shortest path' in interpolation
	glm::vec3 s = glm::mix(this->m_scale, next->m_scale, delta);

	glm::mat4 rotationMatrix = glm::toMat4(r);
	glm::mat4 scaleMatrix = glm::scale(s);
	glm::mat4 translateMatrix = glm::translate(p);

	return translateMatrix * rotationMatrix * scaleMatrix;
}

fwAnimationKeyframe::~fwAnimationKeyframe()
{
}
