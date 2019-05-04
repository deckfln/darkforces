#include "AmbientLight.h"


AmbientLight::AmbientLight(glm::vec3 & _color):
	ambient(_color)
{
}

AmbientLight::AmbientLight(float r, float g, float b):
	ambient(r, g, b)
{
}

AmbientLight::AmbientLight()
{
}

void AmbientLight::set_uniform(glProgram &program)
{
	program.set_uniform("ambientColor", ambient);
}

AmbientLight::~AmbientLight()
{
}
