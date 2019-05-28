#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "glEngine/glProgram.h"
#include "glEngine/glTexture.h"
#include "glEngine/glCubeTexture.h"

class fwUniform
{
	std::string name;
	void *data;
	int type;

public:
	fwUniform();
	fwUniform(std::string _name, glm::vec4 *_v4);
	fwUniform(std::string _name, GLint *id);
	fwUniform(std::string _name, GLfloat *f);
	fwUniform(std::string _name, glTexture *y);
	fwUniform(std::string _name, glCubeTexture *y);
	void set_uniform(glProgram *);
	void *get_value(void);
	~fwUniform();
};

