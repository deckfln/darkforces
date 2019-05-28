#pragma once
#include <glm/glm.hpp>
#include <string>
#include <List>

#include "../glEngine/glProgram.h"
#include "../glEngine/glDepthMap.h"
#include "fwObject3D.h"
#include "fwCamera.h"
#include "fwUniform.h"

class fwLight: public fwObject3D
{
	glDepthMap *shadowMap = nullptr;

protected:
	int type;
	std::string uniform_prefix;
	std::string shader_define;
	glm::vec3 ambient;

	std::list <fwUniform *> uniforms;

	fwCamera *shadowCamera = nullptr;
	GLint m_previousCulling;

public:
	fwLight();
	fwLight(glm::vec3 &_color);
	fwLight(float r, float g, float b);

	std::string &getDefine(void);
	virtual std::string set_uniform(glProgram *program, int index = -1);

	bool castShadow(bool s = true);

	void startShadowMap(void);
	void setShadowCamera(glProgram *program);
	void stopShadowMap(void);

	~fwLight();
};