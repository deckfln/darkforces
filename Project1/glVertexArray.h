#pragma once
#include "glad/glad.h"
#include "glengine/glProgram.h"
#include "Geometry.h"
#include "material.h"

class glVertexArray
{
	GLuint id;
	int count;
	Geometry &geometry;

public:
	glVertexArray(glProgram &, Geometry &, Material &material);
	void bind(void);
	static void unbind(void);
	void draw(GLenum mode);
	~glVertexArray();
};
