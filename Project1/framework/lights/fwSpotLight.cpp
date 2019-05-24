#include "fwSpotLight.h"
#include <string>

fwSpotLight::fwSpotLight(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadatric, float _cutoff, float _outerCutoff):
	fwPointLight(_position, _ambient, _diffuse, _specular, _constant, _linear, _quadatric),
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

fwSpotLight::fwSpotLight()
{
}

std::string fwSpotLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = fwPointLight::set_uniform(program, index);

	program->set_uniform(prefix + ".direction", direction);
	program->set_uniform(prefix + ".cutOff", cutoff);
	program->set_uniform(prefix + ".outerCutOff", outerCutoff);

	return prefix;
}

fwSpotLight::~fwSpotLight()
{
}
