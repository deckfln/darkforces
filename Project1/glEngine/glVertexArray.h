#pragma once
#include "glad/glad.h"
#include "glengine/glProgram.h"
#include "framework/Geometry.h"
#include "framework/material.h"

class glVertexArray
{
	GLuint id;
	int count;
	Geometry &geometry;

public:
	glVertexArray(Geometry &, Material *material);
	void bind(void);
	static void unbind(void);
	void draw(GLenum mode);
	~glVertexArray();
};
