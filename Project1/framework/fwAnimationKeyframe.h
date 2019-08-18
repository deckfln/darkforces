#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class fwAnimationKeyframe
{
	time_t m_time;
	glm::vec3 m_translation = glm::vec3(0);
	glm::quat m_rotation = glm::quat();
	glm::vec3 m_scale = glm::vec3(0);

public:
	fwAnimationKeyframe(time_t time);

	void translation(glm::vec3& position) {		m_translation = position;	};
	void rotation(glm::quat& rotation) {		m_rotation = rotation;	};
	void scale(glm::vec3& scale) {		m_scale = scale;	};

	void interpolate(fwAnimationKeyframe* next, float delta, glm::mat4* target);
	~fwAnimationKeyframe();
};
