#pragma once
#include <iostream>

#include "glTexture.h"

class glCubeTexture: public glTexture
{
protected:
	virtual void _bind(GLint id) { glBindTexture(GL_TEXTURE_CUBE_MAP, id); }

	void *data[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	int width=0;
	int height=0;
	int nrChannels=0;

public:
	glCubeTexture(std::string *files);
	void *get_info(int *width, int *height, int *nrChannels);
	~glCubeTexture();
};

