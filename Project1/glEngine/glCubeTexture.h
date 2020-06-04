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

	bool m_delete_on_exit = false;
	void rotate(char *src, int i, int angle);

public:
	glCubeTexture(std::string *files);
	glCubeTexture(void* files[], int width, int height, int format);
	glCubeTexture(void* image, int width, int height, int format);
	glCubeTexture(int width, int height, GLenum  glformat, GLenum gltype, GLenum glfilter);
	void *get_info(int *width, int *height, int *nrChannels);
	~glCubeTexture();
};

