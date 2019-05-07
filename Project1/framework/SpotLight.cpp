#include "SpotLight.h"
#include <string>

SpotLight::SpotLight(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadatric, float _cutoff, float _outerCutoff):
	PointLight(_position, _ambient, _diffuse, _specular, _constant, _linear, _quadatric),
	direction(_direction),
	cutoff(_cutoff),
	outerCutoff(_outerCutoff)
{
	if (outerCutoff == 0)
		outerCutoff = cutoff;

	uniform_prefix = "spotlights";
	type = 3;
	shader_define = "SPOT_LIGHTS";
}

SpotLight::SpotLight()
{
}

std::string SpotLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = PointLight::set_uniform(program, index);

	program->set_uniform(prefix + ".direction", direction);
	program->set_uniform(prefix + ".cutOff", cutoff);
	program->set_uniform(prefix + ".outerCutOff", outerCutoff);

	return prefix;
}

SpotLight::~SpotLight()
{
}
