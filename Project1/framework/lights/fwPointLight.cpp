#include "fwPointLight.h"

fwPointLight::fwPointLight()
{
}

fwPointLight::fwPointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadatric):
	fwLight(_color),
	diffuse(_diffuse),
	specular(_specular),
	constant(_constant),
	linear(_linear),
	quadratic(_quadatric)

{
	m_Position = _position;
	uniform_prefix = "pointlights";
	type = 2;
	shader_define = "POINT_LIGHTS";
}

std::string fwPointLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = fwLight::set_uniform(program, index);
	program->set_uniform(prefix + ".position", m_Position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);
	program->set_uniform(prefix + ".constant", constant);
	program->set_uniform(prefix + ".linear", linear);
	program->set_uniform(prefix + ".quadratic", quadratic);

	return prefix;
}

fwPointLight::~fwPointLight()
{
}
