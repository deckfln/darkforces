#pragma once
#include "../glad/glad.h"

class glVertexArray
{
	GLuint m_id;

public:
	glVertexArray(void);
	void bind(void);
	static void unbind(void);
	void label(const char* s);
	virtual void draw(GLenum mode, bool indexed, int count);

	//getter/setter
	inline GLuint id(void) { return m_id; };
	~glVertexArray();
};
