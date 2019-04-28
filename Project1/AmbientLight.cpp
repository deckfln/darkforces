#include "AmbientLight.h"


AmbientLight::AmbientLight(glm::vec4 & _color):
	color(_color)
{
}

AmbientLight::AmbientLight(float r, float g, float b, float strenght):
	color(r, g, b, strenght)
{
}

AmbientLight::AmbientLight()
{
}

void AmbientLight::set_uniform(glProgram &program)
{
	program.set_uniform("ambientColor", color);
}

AmbientLight::~AmbientLight()
{
}
