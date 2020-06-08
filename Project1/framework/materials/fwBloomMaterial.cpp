#include "fwBloomMaterial.h"

#include "../fwUniform.h"

fwBloomMaterial::fwBloomMaterial() :
	fwMaterial("shaders/bloom/vertex.glsl", "shaders/bloom/fragment.glsl", "")

{
	m_sourceUniform[0] = new fwUniform("screenTexture", (glTexture *)nullptr);
	addUniform(m_sourceUniform[0]);
	addShader(BLOOM_COPY, "shaders/bloom/copy-back.glsl");
	addShader(BLOOM_EXPAND, "shaders/bloom/expand.glsl");

	m_sourceUniform[1] = new fwUniform("screenTexture2", (glTexture*)nullptr);
	addUniform(m_sourceUniform[1]);
	m_sourceUniform[2] = new fwUniform("screenTexture3", (glTexture*)nullptr);
	addUniform(m_sourceUniform[2]);
	m_sourceUniform[3] = new fwUniform("screenTexture4", (glTexture*)nullptr);
	addUniform(m_sourceUniform[3]);
}

void fwBloomMaterial::setSourceTexture(glTexture *source, int buffer)
{
	m_sourceUniform[buffer]->set(source);
}

fwBloomMaterial::~fwBloomMaterial()
{
};