#pragma once
#include "../fwLight.h"

class fwPointLight: public fwLight
{
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

public:
	fwPointLight();
	fwPointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float constant, float linear, float quadatric);
	std::string set_uniform(glProgram *program, int index = -1);
	~fwPointLight();
};
