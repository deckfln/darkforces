#include "fwPointLight.h"

#include "../fwConstants.h"

#include "../camera/fwCameraPanoramic.h"
#include "../../glEngine/framebuffer/glDepthCubeMap.h"

static fwMaterial materialDepth = fwMaterial("shaders/depthCube/vertex.glsl", "shaders/depthCube/fragment.glsl", "shaders/depthCube/geometry.glsl");
static glProgram* depth_program[3] = { nullptr, nullptr, nullptr };

fwPointLight::fwPointLight()
{
	classID |= FW_POINT_LIGHT;
}

fwPointLight::fwPointLight(glm::vec3 _position, glm::vec3 _color, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadatric):
	fwLight(_color),
	diffuse(_diffuse),
	specular(_specular),
	constant(_constant),
	linear(_linear),
	quadratic(_quadatric)

{
	classID |= FW_POINT_LIGHT;
	m_Position = _position;
	uniform_prefix = "pointlights";
	type = 2;
	shader_define = "POINT_LIGHTS";

	m_materialDepth = &materialDepth;
	m_depth_program = depth_program;
}

bool fwPointLight::castShadow(bool s)
{
	fwObject3D::castShadow(s);
	if (s && m_shadowMap == nullptr) {
		m_shadowMap = new glDepthCubeMap(1024, 1024);
	}
	if (s && m_shadowCamera == nullptr) {
		m_shadowCamera = new fwCameraPanoramic(1, 1, 17.5);
	}
	return s;
}

std::string fwPointLight::set_uniform(glProgram *program, int index)
{
	std::string prefix = fwLight::set_uniform(program, index);
	program->set_uniform(prefix + ".position", m_Position);
	program->set_uniform(prefix + ".diffuse", diffuse);
	program->set_uniform(prefix + ".specular", specular);
	program->set_uniform(prefix + ".constant", constant);
	program->set_uniform(prefix + ".linear", linear);
	program->set_uniform(prefix + ".quadratic", quadratic);

	if (m_shadowCamera) {
		m_shadowCamera->set_uniform(prefix + ".far_plane", "far_plane", program);
	}
	return prefix;
}

fwPointLight::~fwPointLight()
{
}
