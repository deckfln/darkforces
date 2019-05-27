#include "fwLight.h"
#include <string>

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
	if (shadowCamera != nullptr) {
		glm::mat4 matrix = shadowCamera->GetMatrix();
		program->set_uniform(prefix + ".matrix", matrix);
		program->set_uniform(prefix + ".shadowMap", shadowMap->getDepthTexture());
	}

	return prefix;
}

bool fwLight::castShadow(bool flag)
{
	fwObject3D::castShadow(flag);
	if (flag && shadowMap == nullptr) {
		shadowMap = new glDepthMap(1024, 1024);
	}
	return flag;
}

void fwLight::startShadowMap(void)
{
	shadowMap->bind();
	shadowMap->clear();

	glGetIntegerv(GL_CULL_FACE_MODE, &m_previousCulling);
	glCullFace(GL_FRONT);
}

void fwLight::setShadowCamera(glProgram *program)
{
	shadowCamera->translate(position);
	shadowCamera->set_uniformBuffer();
	shadowCamera->bind_uniformBuffer(program);
}

void fwLight::stopShadowMap(void)
{
	shadowMap->unbind();
	glCullFace(m_previousCulling);
}

fwLight::~fwLight()
{
	if (shadowMap) {
		delete shadowMap;
	}
	if (shadowCamera) {
		delete shadowCamera;
	}
}
