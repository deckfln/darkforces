#pragma once
#include "glEngine/glShader.h"
#include <iostream>
#include "Element.h"
class List
{
	Element *first;
	Element *last;
	int length;

public:
	List();
	Element *Add(const std::string source, GLuint type);
	void Remove(glShader *shader);
	~List();
};

