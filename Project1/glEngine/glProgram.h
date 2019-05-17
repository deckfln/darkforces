#pragma once
#include "glad/glad.h"
#include <map>
#include <iostream>
#include "glVertexAttribute.h"
#include "glUniform.h"

class glProgram
{
	// active attributes
	std::map<std::string, glVertexAttribute *> attributes;
	std::map<std::string, glUniform *> uniforms;
	std::map<std::string, int> uniformBufferBindingPoints;

	GLuint id;

public:
	glProgram(void);
	glProgram(const std::string vertexShader, const std::string fragmentShader, const std::string defines);
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
	~glProgram();
};
