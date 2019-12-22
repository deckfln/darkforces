#include "fwMaterialDeferedLights.h"

fwMaterialDeferedLight::fwMaterialDeferedLight() :
	fwMaterial("shaders/quad_vertex.glsl", "shaders/GBuffer/lightning.glsl", "")

{
	m_colorUniform = new fwUniform("gColor", (glTexture *)nullptr);
	m_normalUniform = new fwUniform("gNormal", (glTexture*)nullptr);
	m_worldUniform = new fwUniform("gWorld", (glTexture*)nullptr);
	m_materialUniform = new fwUniform("gMaterial", (glTexture*)nullptr);
	m_pBloomUniform = new fwUniform("bloom", (glTexture*)nullptr);

	addUniform(m_colorUniform);
	addUniform(m_normalUniform);
	addUniform(m_worldUniform);
	addUniform(m_materialUniform);
	addUniform(m_pBloomUniform);
}

void fwMaterialDeferedLight::setSourceTexture(glTexture *color, glTexture *normal, glTexture *world, glTexture *material)
{
	m_colorUniform->set(color);
	m_normalUniform->set(normal);
	m_worldUniform->set(world);
	m_materialUniform->set(material);
}

void fwMaterialDeferedLight::setBloomTexture(glTexture* bloom)
{
	m_pBloomUniform->set(bloom);
}

fwMaterialDeferedLight::~fwMaterialDeferedLight()
{
};