#pragma once
#include "../glad/glad.h"

class glVertexArray
{
	GLuint id;

public:
	glVertexArray(void);
	void bind(void);
	static void unbind(void);
	void label(const char* s);
	virtual void draw(GLenum mode, bool indexed, int count);
	~glVertexArray();
};
