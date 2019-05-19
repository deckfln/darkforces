#pragma once
#include <iostream>
#include "glad/glad.h"
#include "glBufferAttribute.h"

class glInstancedBufferAttribute : public glBufferAttribute
{
	int divisor = 0;

public:
	glInstancedBufferAttribute(std::string _name, int divisor, GLuint _type, void *_data, GLsizei itemSize, GLsizei len, GLuint _sizeof_element, bool delete_on_exit = true);
	int get_divisor(void) { return divisor; };
};
