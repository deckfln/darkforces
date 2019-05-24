#include "fwDirectionLight.h"


fwDirectionLight::fwDirectionLight(glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular):
	fwLight(_color),
	diffuse(_diffuse),
	specular(_specular)
{
	position = _direction;
	uniform_prefix = "dirlights";
	type = 1;
	shader_define = "DIRECTION_LIGHTS";
}

fwDirectionLight::fwDirectionLight()
{
}

std::string fwDirectionLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = fwLight::set_uniform(program);
	program->set_uniform(prefix + ".direction", position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);

	return prefix;
}

fwDirectionLight::~fwDirectionLight()
{
}
