#pragma once
#include <glm/glm.hpp>

#include "glEngine/glProgram.h"
#include "AmbientLight.h"

class DirectionLight: public AmbientLight
{
protected:
	glm::vec3 direction;
	glm::vec3 diffuse;
	glm::vec3 specular;

public:
	DirectionLight();
	DirectionLight(glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular);
	void set_uniform(glProgram &program);
	~DirectionLight();
};