#include "DirectionLight.h"


DirectionLight::DirectionLight(glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular):
	Light(_color),
	diffuse(_diffuse),
	specular(_specular)
{
	position = _direction;
	uniform_prefix = "dirlights";
	type = 1;
	shader_define = "DIRECTION_LIGHTS";
}

DirectionLight::DirectionLight()
{
}

std::string DirectionLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = Light::set_uniform(program);
	program->set_uniform(prefix + ".direction", position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);

	return prefix;
}

DirectionLight::~DirectionLight()
{
}
