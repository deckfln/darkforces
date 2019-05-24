#include "fwLight.h"
#include <string>

fwLight::fwLight(glm::vec3 & _color):
	ambient(_color),
	uniform_prefix("ambient"),
	type(0),
	shader_define("AMBIENT_LIGHT")
{
}

fwLight::fwLight(float r, float g, float b):
	ambient(r, g, b)
{
}

fwLight::fwLight()
{
}

std::string &fwLight::getDefine(void)
{
	return shader_define;
}

std::string fwLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = uniform_prefix;
	if (index >= 0) {
		prefix += "[" + std::to_string(index) + "]";
	}

	program->set_uniform(prefix + ".ambient", ambient);

	return prefix;
}

fwLight::~fwLight()
{
}
