#include "fwPostProcessing.h"

#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "glad/glad.h"

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f, -1.0f,

	-1.0f,  1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f
};

float quadUvs[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

fwPostProcessing::fwPostProcessing(std::string _vertexShader, std::string _fragmentShader, Uniform *_source)
{
	std::string vertex = load_shader_file(_vertexShader, "");
	std::string fragment = load_shader_file(_fragmentShader, "");

	program = new glProgram(vertex, fragment, "", "");

	program->run();
	source = _source;
	source->set_uniform(program);

	Geometry *geometry = new Geometry();
	geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float));
	geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float));

	quad = new glVertexArray(geometry, program);
}

void fwPostProcessing::draw(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);

	program->run();
	glTexture *tex = (glTexture *)source->get_value();
	tex->resetTextureUnit();
	tex->bind();
	quad->draw();
}

fwPostProcessing::~fwPostProcessing()
{
	delete program;
	delete quad;
}
