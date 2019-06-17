#pragma once

#include "../fwMaterial.h"

class fwBloomMaterial : public fwMaterial
{
	fwUniform *m_sourceUniform;
	glm::vec2 m_pixelSize;

public:
	fwBloomMaterial();
	void setSourceTexture(glTexture *source);
	~fwBloomMaterial();
};