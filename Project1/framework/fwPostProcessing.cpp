#include "fwPostProcessing.h"

#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "../glad/glad.h"

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f, -1.0f,

	-1.0f,  1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f
};

float quadUvs[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

fwPostProcessing::fwPostProcessing(std::string _vertexShader, std::string _fragmentShader, fwUniform *_source, std::string defines)
{
	std::string vertex = load_shader_file(_vertexShader, defines);
	std::string fragment = load_shader_file(_fragmentShader, defines);

	program = new glProgram(vertex, fragment, "", "");

	program->run();
	source = _source;

	geometry = new fwGeometry();
	geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
	geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);

	quad = new glVertexArray();
	geometry->enable_attributes(program);
	quad->unbind();
}

fwPostProcessing &fwPostProcessing::addUniform(fwUniform *uniform)
{
	m_uniforms.push_front(uniform);

	return *this;
}

void fwPostProcessing::draw(glTexture *color)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	program->run();

	color->resetTextureUnit();
	source->set(color);
	source->set_uniform(program);

	for (auto uniform : m_uniforms) {
		uniform->set_uniform(program);
	}

	/*
	glTexture *tex = (glTexture *)source->get_value();
	tex->resetTextureUnit();
	tex->bind();
	*/
	geometry->draw(GL_TRIANGLES, quad);

	glEnable(GL_DEPTH_TEST);
}

fwPostProcessing::~fwPostProcessing()
{
	delete program;
	delete quad;
	delete geometry;
}
