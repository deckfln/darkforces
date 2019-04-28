#pragma once
#include "AmbientLight.h"

class DirectionalLight: public AmbientLight
{
public:
	DirectionalLight();
	DirectionalLight(glm::vec3 _position, glm::vec4 _color);
	void set_uniform(glProgram &program);
	~DirectionalLight();
};
