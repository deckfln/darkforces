#pragma once

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

class dfFileSystem;

struct dfVueModel {
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
};

class dfVue {
	std::vector<dfVueModel> m_animations;
	time_t m_currentFrame = 0;
	unsigned int m_fps = 20;	// default value

public:
	dfVue(dfFileSystem* fs, std::string& file, std::string& component);
	glm::mat4* firstFrame(time_t t);
	glm::mat4* nextFrame(time_t t);
	~dfVue();
};