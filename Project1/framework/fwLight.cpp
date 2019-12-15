#include <string>

#include "fwLight.h"
#include "fwConstants.h"
#include "../glEngine/glDepthMap.h"

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

	// upload the m_light space matrix if there is a m_camera
	if (m_shadowCamera != nullptr) {
		glm::mat4 matrix = m_shadowCamera->GetMatrix();
		program->set_uniform(prefix + ".matrix", matrix);
		program->set_uniform(prefix + ".shadowMap", m_shadowMap->getDepthTexture());
	}

	return prefix;
}

bool fwLight::castShadow(bool flag)
{
	fwObject3D::castShadow(flag);
	if (flag && m_shadowMap == nullptr) {
		m_shadowMap = new glDepthMap(1024, 1024);
	}
	return flag;
}

void fwLight::startShadowMap(void)
{
	m_shadowMap->bind();
	m_shadowMap->clear();

	glGetIntegerv(GL_CULL_FACE_MODE, &m_previousCulling);
	glCullFace(GL_FRONT);

	// build the shaders if needed
	if (m_depth_program[0] == nullptr) {
		shadowShaders();
	}
}

void fwLight::runShadowProgram(int i)
{
	m_depth_program[i]->run();

	if (updated) {
		m_shadowCamera->translate(m_Position);
		updated = false;
	}
	m_shadowCamera->set_uniforms(m_depth_program[i]);
}

void fwLight::stopShadowMap(void)
{
	m_shadowMap->unbind();
	glCullFace(m_previousCulling);
}

void fwLight::shadowShaders(void)
{
	m_depth_program[FW_MESH_NORMAL] = new glProgram(
		m_materialDepth->get_shader(VERTEX_SHADER), 
		m_materialDepth->get_shader(FRAGMENT_SHADER), 
		m_materialDepth->get_shader(GEOMETRY_SHADER),
		""
	);

	m_depth_program[FW_MESH_INSTANCED] = new glProgram(
		m_materialDepth->get_shader(VERTEX_SHADER), 
		m_materialDepth->get_shader(FRAGMENT_SHADER), 
		m_materialDepth->get_shader(GEOMETRY_SHADER),
		"#define INSTANCED\n"
	);

	m_depth_program[FW_MESH_SKINNED] = new glProgram(
		m_materialDepth->get_shader(VERTEX_SHADER), 
		m_materialDepth->get_shader(FRAGMENT_SHADER), 
		m_materialDepth->get_shader(GEOMETRY_SHADER),
		"#define SKINNED\n"
	);
}

fwLight::~fwLight()
{
	if (m_shadowMap) {
		delete m_shadowMap;
	}
	if (m_shadowCamera) {
		delete m_shadowCamera;
	}
}
