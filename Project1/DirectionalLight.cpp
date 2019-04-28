#include "DirectionalLight.h"



DirectionalLight::DirectionalLight()
{
}

DirectionalLight::DirectionalLight(glm::vec3 _position, glm::vec4 _color)
{
	position = _position;
	color = _color;
}

void DirectionalLight::set_uniform(glProgram &program)
{
	program.set_uniform("lightPos", position);
	program.set_uniform("lightColor", color);
}

DirectionalLight::~DirectionalLight()
{
}
