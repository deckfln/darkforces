#include "fwBackground.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"

#include "fwCamera.h"
#include "fwGeometry.h"
#include "fwUniform.h"

fwBackground::fwBackground(std::string vertexs, std::string fragments)
{
	std::string vertex = load_shader_file(vertexs, "");
	std::string fragment = load_shader_file(fragments, "");

	program = new glProgram(vertex, fragment, "", "");
	program->run();

	geometry = new fwGeometry();
	cube = new glVertexArray();
}

/**
 * Draw the background after setting up the uniforms
 */
void fwBackground::draw(fwCamera* camera, int renderMode)
{
	if (renderMode == GL_STENCIL_TEST) {
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 255, 0xFF);
		glStencilMask(0x00);

		glDisable(GL_DEPTH_TEST);
	}
	else {
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	}

	program->run();
	camera->bind_uniformBuffer(program);

	setUniforms(program);

	geometry->draw(GL_TRIANGLES, cube);

	if (renderMode == GL_STENCIL_TEST) {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	}
	else {
		glDepthFunc(GL_LESS);  // change depth function so depth test passes when values are equal to depth buffer's content
	}
}

std::string fwBackground::get_shader(const std::string shader_file)
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

fwBackground::~fwBackground()
{
	delete program;
	delete cube;
	delete geometry;
	delete uniform;
}

