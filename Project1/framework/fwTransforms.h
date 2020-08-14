#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Framework
{
	struct fwTransforms
	{
		glm::vec3 m_position;
		glm::vec3 m_scale;
		glm::quat m_quaternion;

		glm::vec3 m_direction;
	};
}