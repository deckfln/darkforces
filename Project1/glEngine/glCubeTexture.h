#pragma once
#include <iostream>

#include "glTexture.h"

class glCubeTexture: public glTexture
{
protected:
	virtual void _bind(GLint id) { glBindTexture(GL_TEXTURE_CUBE_MAP, id); }

	void *m_data[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	int m_width=0;
	int m_height=0;
	int m_nrChannels=0;

public:
	glCubeTexture(std::string *files);
	glCubeTexture(int width, int height, GLenum  glformat, GLenum gltype, GLenum glfilter);
	void *get_info(int *width, int *height, int *nrChannels);
	~glCubeTexture();
};

