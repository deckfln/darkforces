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
	dfVue(dfFileSystem* fs, const std::string& file, const std::string& component);
	glm::mat4* firstFrame(time_t t);
	glm::mat4* nextFrame(time_t t);
	time_t currentFrame(void);			// return the time of the current frame
	void currentFrame(time_t t);		// set a new frame time
	~dfVue();
};