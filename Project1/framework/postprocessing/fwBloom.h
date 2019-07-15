#pragma once

#include "../../glEngine/glProgram.h"
#include "../../glEngine/glVertexArray.h"
#include "../../glEngine/glColorMap.h"

#include "../fwUniform.h"
#include "../fwGeometry.h"

class fwBloom
{
	GLuint quadVAO = -1;
	GLuint quadVBO = -1;
	glVertexArray *quad = nullptr;
	fwGeometry *geometry = nullptr;
	fwUniform *source = nullptr;
	glColorMap *m_pingBloomBuffer[3] = { nullptr, nullptr, nullptr };

public:
	fwBloom();
	void draw(glColorMap *map);
	~fwBloom();
};