#include "Light.h"


Light::Light(glm::vec3 & _color):
	ambient(_color)
{
}

Light::Light(float r, float g, float b):
	ambient(r, g, b)
{
}

Light::Light()
{
}

void Light::set_uniform(glProgram &program)
{
	program.set_uniform("light.ambient", ambient);
}

Light::~Light()
{
}
