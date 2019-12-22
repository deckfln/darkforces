#pragma once
#include "../fwLight.h"

class fwPointLight: public fwLight
{
	glm::vec3 m_diffuse = glm::vec3(0);
	glm::vec3 m_specular = glm::vec3(0);

	float m_constant = 0;
	float m_linear = 0;
	float m_quadratic = 0;
	float m_radius = 0;

public:
	fwPointLight();
	fwPointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float constant, float linear, float quadatric);
	bool castShadow(bool s);
	std::string set_uniform(glProgram *program, int index = -1);
	~fwPointLight();
};
