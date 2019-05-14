#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "glEngine/glProgram.h"
#include "glEngine/glTexture.h"
#include "glEngine/glCubeTexture.h"

class Uniform
{
	std::string name;
	void *data;
	int type;

public:
	Uniform();
	Uniform(std::string _name, glm::vec4 *_v4);
	Uniform(std::string _name, GLint *id);
	Uniform(std::string _name, GLfloat *f);
	Uniform(std::string _name, glTexture *y);
	Uniform(std::string _name, glCubeTexture *y);
	void set_uniform(glProgram *);
	void *get_value(void);
	~Uniform();
};

