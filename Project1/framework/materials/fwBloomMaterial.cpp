#include "fwBloomMaterial.h"

fwBloomMaterial::fwBloomMaterial() :
	fwMaterial("shaders/bloom/vertex.glsl", "shaders/bloom/fragment.glsl", "")

{
	m_sourceUniform = new fwUniform("screenTexture", (glTexture *)nullptr);
	addUniform(m_sourceUniform);
	addShader(BLOOM_COPY, "shaders/bloom/copy-back.glsl");
	addShader(BLOOM_EXPAND, "shaders/bloom/expand.glsl");
}

void fwBloomMaterial::setSourceTexture(glTexture *source)
{
	m_sourceUniform->set(source);
}

fwBloomMaterial::~fwBloomMaterial()
{
};