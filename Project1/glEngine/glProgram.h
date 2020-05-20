#pragma once
#include "../glad/glad.h"
#include <map>
#include <iostream>
#include "glVertexAttribute.h"
#include "glUniform.h"
#include "glTexture.h"

std::string load_shader_file(const std::string shader_file, std::string defines, std::map<std::string,std::string> *variables=nullptr);

class glUniformBlock;

class glProgram
{
	// active attributes
	std::map<std::string, glVertexAttribute *> attributes;
	std::map<std::string, glUniform *> uniforms;
	std::map<std::string, int> uniformBufferBindingPoints;

	int m_nbUniformBlocks = 0;
	std::map<std::string, glUniformBlock*> m_uniformBlocks;

	GLuint id;

public:
	glProgram(void);
	glProgram(const std::string vertexShader, const std::string fragmentShader, const std::string geometryShader, const std::string defines);
	GLuint getID(void);
	void run(void);
	glVertexAttribute *get_attribute(const std::string name);
	glUniform *get_uniform(std::string name);
	bool bindBufferAttribute(std::string name, int bindingPoint);
	void set_uniform(const std::string name, GLint id);
	void set_uniform(const std::string name, GLfloat f);
	void set_uniform(const std::string name, GLfloat r, GLfloat g, GLfloat b);
	void set_uniform(const std::string name, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void set_uniform(const std::string name, glm::mat4 &mat4);
	void set_uniform(const std::string name, glm::vec4 &vec4);
	void set_uniform(const std::string name, glm::vec3 &vec3);
	void set_uniform(const std::string name, glm::vec2 &vec2);
	void set_uniform(const std::string name, glTexture *);
	void set_uniform(const std::string name, glm::mat4 *, int size=1);
	void set_uniform(const std::string name, glm::vec4*, int size = 1);
	~glProgram();
};
