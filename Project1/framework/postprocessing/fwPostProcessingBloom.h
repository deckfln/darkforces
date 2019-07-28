#pragma once

#include "../../glEngine/glProgram.h"
#include "../../glEngine/glVertexArray.h"
#include "../../glEngine/glColorMap.h"

#include "../fwUniform.h"
#include "../fwGeometry.h"

class fwPostProcessingBloom
{
	GLuint quadVAO = -1;
	GLuint quadVBO = -1;
	glVertexArray *quad = nullptr;
	fwGeometry *geometry = nullptr;
	fwUniform *source = nullptr;
	glColorMap *m_pingBloomBuffer[3] = { nullptr, nullptr, nullptr };
	glTexture* m_pBloom_texture = nullptr;

public:
	fwPostProcessingBloom(int _width, int _height);
	glTexture* get_bloom_texture(void);
	void draw(glColorMap *map);
	~fwPostProcessingBloom();
};