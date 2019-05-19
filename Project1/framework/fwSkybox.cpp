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
	// positions          
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

fwSkybox::fwSkybox(std::string *textures)
{
	texture = new glCubeTexture(textures);

	std::string vertex = get_shader("shaders/cubemap_vertex.glsl");
	std::string fragment = get_shader("shaders/cubemap_fragment.glsl");

	program = new glProgram(vertex, fragment, "", "");

	program->run();
	uniform = new Uniform("skybox", texture);
	uniform->set_uniform(program);

	geometry = new Geometry();
	geometry->addVertices("aPos", skyboxVertices, 3, sizeof(skyboxVertices), sizeof(float), false);

	cube = new glVertexArray();
	geometry->enable_attributes(program);
	cube->unbind();
}

void fwSkybox::draw(Camera *camera)
{
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	program->run();
	camera->set_uniforms(program);
	/*
	glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	glm::mat4 projection = camera->GetProjectionMatrix();
	program->set_uniform("view", view);
	program->set_uniform("projection", projection);
	*/
	// camera->set_uniforms(program);
	texture->resetTextureUnit();
	texture->bind();

	geometry->draw(GL_TRIANGLES, cube);
	glDepthFunc(GL_LESS); // set depth function back to default
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
