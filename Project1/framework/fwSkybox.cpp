#include "fwSkybox.h"

#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../glEngine/glCubeTexture.h"

#include "../glad/glad.h"
#include "../include/stb_image.h"

float skyboxVertices[] = {
	// m_positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

fwSkybox::fwSkybox(std::string* textures)
{
	texture = new glCubeTexture(textures);

	std::string vertex = load_shader_file("shaders/cubemap_vertex.glsl", "");
	std::string fragment = load_shader_file("shaders/cubemap_fragment.glsl", "");

	program = new glProgram(vertex, fragment, "", "");

	program->run();
	uniform = new fwUniform("skybox", texture);
	//TODO : the skybox textureID == 1 here
	uniform->set_uniform(program);

	geometry = new fwGeometry();
	geometry->addVertices("aPos", skyboxVertices, 3, sizeof(skyboxVertices), sizeof(float), false);

	cube = new glVertexArray();
	geometry->enable_attributes(program);
	cube->unbind();
}

void fwSkybox::draw(fwCamera *camera, int renderMode)
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
	/*
	glm::mat4 view = glm::mat4(glm::mat3(m_camera->GetViewMatrix()));
	glm::mat4 projection = m_camera->GetProjectionMatrix();
	program->set_uniform("view", view);
	program->set_uniform("projection", projection);
	*/
	// m_camera->set_uniforms(program);
	program->set_uniform("skybox", texture);

	geometry->draw(GL_TRIANGLES, cube);

	if (renderMode == GL_STENCIL_TEST) {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	}
	else {
		glDepthFunc(GL_LESS);  // change depth function so depth test passes when values are equal to depth buffer's content
	}
}

std::string fwSkybox::get_shader(const std::string shader_file)
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

fwSkybox::~fwSkybox()
{
	delete program;
	delete cube;
	delete geometry;
	delete uniform;
}
