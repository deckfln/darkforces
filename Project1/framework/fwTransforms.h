#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Framework
{
	struct fwTransforms
	{
		glm::vec3 m_position = glm::vec3(0);
		glm::vec3 m_scale = glm::vec3(1);
		glm::quat m_quaternion = glm::quat(1, 0, 0, 0);
	};
}