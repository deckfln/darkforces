#pragma once

#include <list>

#include "../../glEngine/glProgram.h"
#include "../../glEngine/glVertexArray.h"
#include "../../glEngine/glColorMap.h"
#include "../../glEngine/glGBuffer.h"

#include "../fwUniform.h"
#include "../fwGeometry.h"
#include "../lights/fwDirectionLight.h"

class fwPPDirectLight
{
	GLuint quadVAO = -1;
	GLuint quadVBO = -1;
	glVertexArray *quad = nullptr;
	fwGeometry *geometry = nullptr;
	fwUniform *source = nullptr;
	glColorMap* m_colorMap = nullptr;

public:
	fwPPDirectLight();
	glColorMap *draw(glGBuffer*map, std::list <fwDirectionLight *>lights);
	~fwPPDirectLight();
};