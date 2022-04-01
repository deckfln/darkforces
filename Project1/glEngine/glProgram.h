#pragma once
#include "../glad/glad.h"
#include <map>
#include <iostream>
#include "glVertexAttribute.h"
#include "glUniform.h"
#include "glTexture.h"

std::string load_shader_file(const std::string& shader_file, 
	const std::string& defines, 
	std::map<std::string,std::string> *variables=nullptr
);

class glUniformBlock;

class glProgram
{
	// active attributes
	std::map<std::string, glVertexAttribute *> attributes;
	std::map<std::string, glUniform *> uniforms;
	std::map<std::string, int> uniformBufferBindingPoints;

	int m_nbUniformBlocks = 0;
	std::map<std::string, glUniformBlock*> m_uniformBlocks;

	GLuint m_id;

public:
	glProgram(void);
	glProgram(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& defines);
	GLuint getID(void);
	inline uint32_t id(void) { return m_id; };
	void run(void);
	glVertexAttribute *get_attribute(const std::string& name);
	glUniform *get_uniform(const std::string& name);
	bool bindBufferAttribute(const std::string& name, int bindingPoint);
	void set_uniform(const std::string& name, GLint id);
	void set_uniform(const std::string& name, GLfloat f);
	void set_uniform(const std::string& name, GLfloat r, GLfloat g, GLfloat b);
	void set_uniform(const std::string& name, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	void set_uniform(const std::string& name, const glm::mat4 &mat4);
	void set_uniform(const std::string& name, const glm::vec4 &vec4);
	void set_uniform(const std::string& name, const glm::vec3 &vec3);
	void set_uniform(const std::string& name, const glm::vec2 &vec2);
	void set_uniform(const std::string& name, glTexture *);
	void set_uniform(const std::string& name, glm::mat4 *, int size=1);
	void set_uniform(const std::string& name, glm::vec4*, int size = 1);
	~glProgram();
};
