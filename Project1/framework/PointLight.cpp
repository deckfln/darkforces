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
	uniform_prefix = "pointlights";
	type = 2;
	shader_define = "POINT_LIGHTS";
}

std::string PointLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = Light::set_uniform(program, index);
	program->set_uniform(prefix + ".position", position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);
	program->set_uniform(prefix + ".constant", constant);
	program->set_uniform(prefix + ".linear", linear);
	program->set_uniform(prefix + ".quadratic", quadratic);

	return prefix;
}

PointLight::~PointLight()
{
}
