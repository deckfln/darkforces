#pragma once

#include <glm/vec3.hpp>

class alListener
{
public:
	alListener();
	void position(const glm::vec3& position);
	void orientation(const glm::vec3& at, const glm::vec3& up);
	~alListener();
};
extern alListener g_Listener;
