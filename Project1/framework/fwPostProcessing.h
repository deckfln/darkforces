#pragma once
#include <list>

#include "framework/fwUniform.h"

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
	fwUniform *source;

public:
	fwPostProcessing(std::string _vertexShader, std::string _fragmentShader, fwUniform *_source);
	void draw(void);
	~fwPostProcessing();
};