#include "fwMaterialDirectionalLight.h"

fwMaterialDirectionalLight::fwMaterialDirectionalLight() :
	fwMaterial("shaders/quad_vertex.glsl", "shaders/GBuffer/directional_light.glsl", "")

{
	m_colorUniform = new fwUniform("gColor", (glTexture *)nullptr);
	m_normalUniform = new fwUniform("gNormal", (glTexture*)nullptr);
	m_worldUniform = new fwUniform("gWorld", (glTexture*)nullptr);
	m_materialUniform = new fwUniform("gMaterial", (glTexture*)nullptr);
	addUniform(m_colorUniform);
	addUniform(m_normalUniform);
	addUniform(m_worldUniform);
	addUniform(m_materialUniform);
}

void fwMaterialDirectionalLight::setSourceTexture(glTexture *color, glTexture *normal, glTexture *world, glTexture *material)
{
	m_colorUniform->set(color);
	m_normalUniform->set(normal);
	m_worldUniform->set(world);
	m_materialUniform->set(material);
}

fwMaterialDirectionalLight::~fwMaterialDirectionalLight()
{
};