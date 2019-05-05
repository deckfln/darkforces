#include "SpotLight.h"


SpotLight::SpotLight(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadatric, float _cutoff, float _outerCutoff):
	PointLight(_position, _ambient, _diffuse, _specular, _constant, _linear, _quadatric),
	direction(_direction),
	cutoff(_cutoff),
	outerCutoff(_outerCutoff)
{
	if (outerCutoff == 0)
		outerCutoff = cutoff;
}

SpotLight::SpotLight()
{
}

void SpotLight::set_uniform(glProgram &program)
{
	PointLight::set_uniform(program);
	program.set_uniform("light.direction", direction);
	program.set_uniform("light.cutOff", cutoff);
	program.set_uniform("light.outerCutOff", outerCutoff);
}

SpotLight::~SpotLight()
{
}
