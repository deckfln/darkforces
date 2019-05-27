#include "fwDirectionLight.h"

#include "../camera/fwOrthographicCamera.h"


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

bool fwDirectionLight::castShadow(bool s)
{
	fwLight::castShadow(s);
	if (s && shadowCamera == nullptr) {
		shadowCamera = new fwOrthographicCamera(10, 1, 7.5);
	}
	return s;
}

std::string fwDirectionLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = fwLight::set_uniform(program, index);
	program->set_uniform(prefix + ".direction", position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);

	return prefix;
}

fwDirectionLight::~fwDirectionLight()
{
}
