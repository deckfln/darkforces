#pragma once
#include "../glad/glad.h"
#include "glVertexArray.h"

class glInstancedVertexArray : public glVertexArray
{
	int instances;

public:
	glInstancedVertexArray(int instances);
	void set_instances(int instances);
	virtual void draw(GLenum mode, bool indexed, int count);
};
