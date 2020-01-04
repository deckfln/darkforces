#pragma once
#include <glm/glm.hpp>
#include <string>
#include <List>

#include "../glEngine/glProgram.h"
#include "../glEngine/glDepthMap.h"
#include "fwObject3D.h"
#include "fwCamera.h"
#include "fwUniform.h"
#include "fwMaterial.h"

class fwLight: public fwObject3D
{
protected:
	int type = 0;
	std::string uniform_prefix;
	std::string shader_define;
	glm::vec3 ambient = glm::vec3(0);

	std::list <fwUniform *> uniforms;

	fwCamera *m_shadowCamera = nullptr;
	GLint m_previousCulling = 0;
	glDepthMap *m_shadowMap = nullptr;

	fwMaterial *m_materialDepth = nullptr;
	glProgram **m_depth_program = nullptr;

public:
	fwLight();
	fwLight(glm::vec3 &_color);
	fwLight(float r, float g, float b);

	std::string &getDefine(void);
	virtual std::string set_uniform(glProgram *program, int index = -1);
	void shadowShaders(void);

	virtual bool castShadow(bool s);

	void startShadowMap(void);
	void runShadowProgram(int i);
	glProgram* getShadowProgram(int i) { return m_depth_program[i]; };
	void stopShadowMap(void);

	~fwLight();
};