#pragma once
#include <glm/glm.hpp>
#include <string>
#include <List>

#include "glEngine/glProgram.h"
#include "fwObject3D.h"
#include "Uniform.h"

class Light: public fwObject3D
{
protected:
	int type;
	std::string uniform_prefix;
	std::string shader_define;
	glm::vec3 ambient;

	std::list <Uniform *> uniforms;

public:
	Light();
	Light(glm::vec3 &_color);
	Light(float r, float g, float b);
	std::string &getDefine(void);
	virtual std::string set_uniform(glProgram *program, int index = -1);
	~Light();
};