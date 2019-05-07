#include "Light.h"
#include <string>

Light::Light(glm::vec3 & _color):
	ambient(_color),
	uniform_prefix("ambient"),
	type(0),
	shader_define("AMBIENT_LIGHT")
{
}

Light::Light(float r, float g, float b):
	ambient(r, g, b)
{
}

Light::Light()
{
}

std::string &Light::getDefine(void)
{
	return shader_define;
}

std::string Light::set_uniform(glProgram *program, int index)
{
	std::string prefix = uniform_prefix;
	if (index >= 0) {
		prefix += "[" + std::to_string(index) + "]";
	}

	program->set_uniform(prefix + ".ambient", ambient);

	return prefix;
}

Light::~Light()
{
}
