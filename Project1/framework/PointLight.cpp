#include "framework/PointLight.h"



PointLight::PointLight()
{
}

PointLight::PointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadatric):
	Light(_color),
	diffuse(_diffuse),
	specular(_specular),
	constant(_constant),
	linear(_linear),
	quadratic(_quadatric)

{
	position = _position;
}

void PointLight::set_uniform(glProgram &program)
{
	Light::set_uniform(program);
	program.set_uniform("light.position", position);
	program.set_uniform("light.diffuse", diffuse);
	program.set_uniform("light.specular", specular);
	program.set_uniform("light.constant", constant);
	program.set_uniform("light.linear", linear);
	program.set_uniform("light.quadratic", quadratic);
}

PointLight::~PointLight()
{
}
