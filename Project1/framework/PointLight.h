#pragma once
#include "Light.h"

class PointLight: public Light
{
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

public:
	PointLight();
	PointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float constant, float linear, float quadatric);
	void set_uniform(glProgram &program);
	~PointLight();
};
