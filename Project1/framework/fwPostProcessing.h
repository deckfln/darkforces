#pragma once
#include <list>

#include "framework/Uniform.h"

#include "glEngine/glProgram.h"
#include "glEngine/glVertexArray.h"

class fwPostProcessing
{
	GLuint quadVAO;
	GLuint quadVBO;
	glProgram *program;
	glVertexArray *quad;
	Uniform *source;

	std::string get_shader(const std::string shader_file);

public:
	fwPostProcessing(std::string _vertexShader, std::string _fragmentShader, Uniform *_source);
	void draw(void);
	~fwPostProcessing();
};