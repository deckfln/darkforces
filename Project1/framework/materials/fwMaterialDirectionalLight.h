#pragma once

#include "../fwMaterial.h"

class fwMaterialDirectionalLight : public fwMaterial
{
	fwUniform *m_colorUniform = nullptr;
	fwUniform *m_normalUniform = nullptr;
	fwUniform *m_worldUniform = nullptr;
	fwUniform *m_materialUniform = nullptr;
	fwUniform* m_pBloomUniform = nullptr;

	glm::vec2 m_pixelSize = glm::vec2(0);

public:
	fwMaterialDirectionalLight();
	void setSourceTexture(glTexture* color, glTexture* normal, glTexture* world, glTexture* material);
	void setBloomTexture(glTexture* bloom);
	~fwMaterialDirectionalLight();
};