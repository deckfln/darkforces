#pragma once

#include <vector>

#include "../fwCamera.h"

class fwCameraPanoramic : public fwCamera
{
	float m_far = 0;
	glm::mat4 m_transforms[6] = {glm::mat4(1), glm::mat4(1), glm::mat4(1), glm::mat4(1), glm::mat4(1), glm::mat4(1)};

public:
	fwCameraPanoramic(float aspect, float near, float far);
	void update(void);
	void set_uniforms(glProgram* program);
	void set_uniform(std::string name, std::string attr, glProgram* program);
	~fwCameraPanoramic();
};