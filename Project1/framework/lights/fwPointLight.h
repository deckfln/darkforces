#pragma once
#include "../fwLight.h"

class fwPointLight: public fwLight
{
	glm::vec3 diffuse = glm::vec3(0);
	glm::vec3 specular = glm::vec3(0);

	float constant = 0;
	float linear = 0;
	float quadratic = 0;

public:
	fwPointLight();
	fwPointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float constant, float linear, float quadatric);
	bool castShadow(bool s);
	std::string set_uniform(glProgram *program, int index = -1);
	~fwPointLight();
};
