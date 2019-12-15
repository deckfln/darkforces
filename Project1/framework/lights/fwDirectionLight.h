#pragma once
#include <glm/glm.hpp>

#include "../../glEngine/glProgram.h"
#include "../fwLight.h"

class fwDirectionLight: public fwLight
{
protected:
	glm::vec3 diffuse = glm::vec3(0);
	glm::vec3 specular = glm::vec3(0);

public:
	fwDirectionLight();
	fwDirectionLight(glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular);
	std::string set_uniform(glProgram *program, int index = -1);
	void shadowShaders(void);
	bool castShadow(bool s);
	~fwDirectionLight();
};