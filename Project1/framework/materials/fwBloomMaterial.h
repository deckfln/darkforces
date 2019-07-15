#pragma once

#include "../fwMaterial.h"

enum BloomShaders { BLOOM_COPY = 3, BLOOM_EXPAND = 4};

class fwBloomMaterial : public fwMaterial
{
	fwUniform *m_sourceUniform = nullptr;
	glm::vec2 m_pixelSize = glm::vec2(0);

public:
	fwBloomMaterial();
	void setSourceTexture(glTexture *source);
	~fwBloomMaterial();
};