#include "fwDirectionLight.h"

#include "../fwConstants.h"

#include "../camera/fwOrthographicCamera.h"

static fwMaterial materialDepth = fwMaterial("shaders/depth/vertex.glsl", "shaders/depth/fragment.glsl", "");
static glProgram *depth_program[3] = { nullptr, nullptr, nullptr };

fwDirectionLight::fwDirectionLight(glm::vec3 _direction, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular):
	fwLight(_color),
	diffuse(_diffuse),
	specular(_specular)
{
	classID |= FW_DIRECTIONAL_LIGHT;
	m_Position = _direction;
	uniform_prefix = "dirlights";
	type = 1;
	shader_define = "DIRECTION_LIGHTS";

	m_materialDepth = &materialDepth;
	m_depth_program = depth_program;
}

fwDirectionLight::fwDirectionLight()
{
}

bool fwDirectionLight::castShadow(bool s)
{
	fwLight::castShadow(s);
	if (s && m_shadowCamera == nullptr) {
		m_shadowCamera = new fwOrthographicCamera(10, 1, 17.5);
	}
	return s;
}

std::string fwDirectionLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = fwLight::set_uniform(program, index);
	program->set_uniform(prefix + ".direction", m_Position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);

	return prefix;
}

fwDirectionLight::~fwDirectionLight()
{
}
