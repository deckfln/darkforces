#include "fwBloom.h"

#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "../../glad/glad.h"
#include "../materials/fwBloomMaterial.h"

static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f, -1.0f,

	-1.0f,  1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f
};

static float quadUvs[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

static fwBloomMaterial Bloom;
static glProgram *Bloom_program = nullptr;
static glProgram *Expand_program = nullptr;
static glProgram *Copy_program = nullptr;

fwBloom::fwBloom(void)
{
}

void fwBloom::draw(glColorMap *colorMap)
{
	if (Bloom_program == nullptr) {
		Bloom_program = new glProgram(Bloom.get_vertexShader(), Bloom.get_fragmentShader(), "", "");
		Expand_program = new glProgram(Bloom.get_vertexShader(), Bloom.get_shader(BLOOM_COPY), "", "");
		Copy_program = new glProgram(Bloom.get_vertexShader(), Bloom.get_shader(BLOOM_EXPAND), "", "");

		geometry = new fwGeometry();
		quad = new glVertexArray();

		geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
		geometry->enable_attributes(Bloom_program);
		quad->unbind();

		glm::vec2 size = colorMap->size();
		m_pingBloomBuffer[0] = new glColorMap(size.x, size.y, 1);
		m_pingBloomBuffer[1] = new glColorMap(size.x, size.y, 1);
		m_pingBloomBuffer[2] = new glColorMap(size.x, size.y, 1);
	}

	glDisable(GL_DEPTH_TEST);								// disable depth test so screen-space quad isn't discarded due to depth test.

	// copy into bloom buffer and expand the objects
	Expand_program->run();
	glTexture::resetTextureUnit();

	m_pingBloomBuffer[0]->bind();								// write out to the ping pong buffer
	m_pingBloomBuffer[0]->clear();
	Bloom.setSourceTexture(colorMap->getColorTexture(1));	// set color buffer 1 as source of the bloom program
	Bloom.set_uniforms(Expand_program);
	geometry->draw(GL_TRIANGLES, quad);
	/*
	Bloom_program->run();
	int source = 0;

	for (int i = 0; i < 1; i++) {
		glTexture::resetTextureUnit();
		// vertical blur
		m_pingBloomBuffer[1]->bind();								// write out to the ping pong buffer
		m_pingBloomBuffer[1]->clear();
		Bloom.setSourceTexture(m_pingBloomBuffer[source]->getColorTexture(0));	// set color buffer 1 as source of the bloom program
		Bloom.set_uniforms(Bloom_program);
		Bloom_program->set_uniform("horizontal", 1.0f);
		Bloom_program->set_uniform("vertical", 0.0f);
		geometry->draw(GL_TRIANGLES, quad);

		source = 1;
		m_pingBloomBuffer[0]->bind();								// write out to the ping pong buffer
		m_pingBloomBuffer[0]->clear();
		Bloom.setSourceTexture(m_pingBloomBuffer[source]->getColorTexture(0));	// set color buffer 1 as source of the bloom program
		Bloom.set_uniforms(Bloom_program);
		Bloom_program->set_uniform("horizontal", 0.0f);
		Bloom_program->set_uniform("vertical", 1.0f);
		geometry->draw(GL_TRIANGLES, quad);

		source = 0;
	}
	*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Copy_program->run();
	glTexture::resetTextureUnit();
	colorMap->bind();										// write out to the source buffer
	Bloom.setSourceTexture(m_pingBloomBuffer[0]->getColorTexture(0));	// set color buffer 1 as source of the bloom program
	Bloom.set_uniforms(Copy_program);
	geometry->draw(GL_TRIANGLES, quad);

	glEnable(GL_DEPTH_TEST); 
	glDisable(GL_BLEND);
}

fwBloom::~fwBloom()
{
}
