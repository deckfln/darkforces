#pragma once
#include <list>

#include "framework/Uniform.h"

#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"
#include "fwGeometry.h"

class fwPostProcessing
{
	GLuint quadVAO;
	GLuint quadVBO;
	glProgram *program;
	glVertexArray *quad;
	fwGeometry *geometry;
	Uniform *source;

public:
	fwPostProcessing(std::string _vertexShader, std::string _fragmentShader, Uniform *_source);
	void draw(void);
	~fwPostProcessing();
};