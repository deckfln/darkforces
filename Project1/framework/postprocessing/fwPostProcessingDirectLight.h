#pragma once

#include <list>

#include "../../glEngine/glProgram.h"
#include "../../glEngine/glVertexArray.h"
#include "../../glEngine/glColorMap.h"
#include "../../glEngine/glGBuffer.h"

#include "../fwUniform.h"
#include "../fwGeometry.h"
#include "../lights/fwDirectionLight.h"

class fwPostProcessingDirectLight
{
	GLuint quadVAO = -1;
	GLuint quadVBO = -1;
	glVertexArray* quad[2] = { nullptr, nullptr };
	fwGeometry *geometry = nullptr;
	fwUniform *source = nullptr;
	glColorMap* m_colorMap = nullptr;
	glTexture* m_pBloomTexture = nullptr;
	void drawLight(std::list <fwDirectionLight*>lights, glGBuffer *colorMap, glProgram* program, glVertexArray* quad);

public:
	fwPostProcessingDirectLight(glTexture* bloom_texture = nullptr);
	glColorMap *draw(glGBuffer*map, std::list <fwDirectionLight *>lights);
	~fwPostProcessingDirectLight();
};