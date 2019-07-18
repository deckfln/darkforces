#pragma once
#include <list>

#include "fwUniform.h"

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

	std::list <fwUniform *> m_uniforms;

public:
	fwPostProcessing(std::string _vertexShader, std::string _fragmentShader, fwUniform *_source, std::string defines);
	fwPostProcessing &addUniform(fwUniform *uniform);
	void draw(glTexture *color);
	~fwPostProcessing();
};