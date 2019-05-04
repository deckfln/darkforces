#include "DirectionLight.h"


DirectionLight::DirectionLight(glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular):
	AmbientLight(_color),
	diffuse(_diffuse),
	specular(_specular)
{
	position = _direction;
}

DirectionLight::DirectionLight()
{
}

void DirectionLight::set_uniform(glProgram &program)
{
	program.set_uniform("light.direction", position);
	program.set_uniform("light.ambient", ambient);
	program.set_uniform("light.diffuse", diffuse);
	program.set_uniform("light.specular", specular);
}

DirectionLight::~DirectionLight()
{
}
