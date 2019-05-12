#include "fwPostProcessing.h"

#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>

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
	std::string vertex = get_shader(_vertexShader);
	std::string fragment = get_shader(_fragmentShader);

	program = new glProgram(vertex, fragment, "");

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

std::string fwPostProcessing::get_shader(const std::string shader_file)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string code;
	std::ifstream file;

	// ensure ifstream objects can throw exceptions:
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		file.open(shader_file);
		std::stringstream vShaderStream;
		// read file's buffer contents into streams
		vShaderStream << file.rdbuf();
		// close file handlers
		file.close();
		// convert stream into string
		code = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER " << shader_file << " ::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		exit(-1);
	}

	return code;
}

fwPostProcessing::~fwPostProcessing()
{
	delete program;
	delete quad;
}
