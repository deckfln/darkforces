#include "glCubeTexture.h"

#include "../glad/glad.h"
#include "../include/stb_image.h"

/**
 * rotate image i by angle
 */
void glCubeTexture::rotate(char *src, int i, int angle)
{
	char* dst = (char *)m_data[i];
	int s = m_width * m_height * m_nrChannels;
	int p, p1;

	switch (angle) {
	case 0:
		for (auto p = s-1; p >= 0; p--) {
				dst[p] = src[p];
		}
		break;
	case 90:
		for (auto y = 0; y < m_height; y++) {
			for (auto x = 0; x < m_width; x++) {
				p = y * m_height + x;
				p1 = x * m_width + y;
				for (auto k = 0; k < m_nrChannels; k++) {
					dst[p * m_nrChannels + k] = src[p1 * m_nrChannels + k];
				}
			}
		}
		break;
	case 180:
		p1 = 0;
		for (auto p = s - 3; p >= 0; p-=3) {
			dst[p] = src[p1++];
			dst[p+1] = src[p1++];
			dst[p+2] = src[p1++];
		}
		break;
	}
}

/**
 * Create from a table of files
 */
glCubeTexture::glCubeTexture(std::string *files)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (GLuint i = 0; i < 6; i++)
	{
		m_data[i] = stbi_load(files[i].c_str(), &m_width, &m_height, &m_nrChannels, 0);
		GLuint glformat = GL_RGB;

		switch (m_nrChannels) {
		case 3: glformat = GL_RGB; break;
		case 4: glformat = GL_RGBA; break;
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, glformat, m_width, m_height, 0, glformat, GL_UNSIGNED_BYTE, m_data[i]
		);
	}

	m_delete_on_exit = true;
}

/**
 * Create from a table of images
 */
glCubeTexture::glCubeTexture(void* files[], int width, int height, int format):
	m_width(width),
	m_height(height),
	m_nrChannels(format)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (GLuint i = 0; i < 6; i++)
	{
		m_data[i] = files[i];
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, m_nrChannels, m_width, m_height, 0, m_nrChannels, GL_UNSIGNED_BYTE, m_data[i]
		);
	}
}

/**
 * Create from single image
 */
glCubeTexture::glCubeTexture(void* image, int width, int height, int format) :
	m_width(width),
	m_height(height)
{
	switch (format) {
	case GL_RGB:
		m_nrChannels = 3;
		break;
	case GL_RGBA:
		m_nrChannels = 4;
		break;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int s = width * height * m_nrChannels;

	for (GLuint i = 0; i < 6; i++)
	{
		// clone and rotate the image
		m_data[i] = new char[s]();
		rotate((char*)image, i, 0);

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, m_data[i]
		);
	}

	m_delete_on_exit = true;
}

/**
 * Create with empty textures
 */
glCubeTexture::glCubeTexture(int width, int height, GLenum  glformat, GLenum gltype, GLenum glfilter) :
	m_width(width),
	m_height(height)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, glfilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, glfilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	switch (glformat) {
	case GL_R: m_nrChannels = 1; break;
	case GL_RG: m_nrChannels = 2; break;
	case GL_RGB: m_nrChannels = 3; break;
	case GL_RGBA: m_nrChannels = 4; break;
	case GL_DEPTH_COMPONENT: m_nrChannels = 1; break;
	}
	for (GLuint i = 0; i < 6; i++)
	{
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, glformat, width, height, 0, glformat, gltype, NULL
		);
	}
}

void *glCubeTexture::get_info(int *_width, int *_height, int *_nrChannels)
{
	*_width = m_width;
	*_height = m_height;
	*_nrChannels = m_nrChannels;
	return m_data;
}

glCubeTexture::~glCubeTexture()
{
	if (m_delete_on_exit) {
		for (int i = 0; i < 6; i++) {
			if (m_data[i] != nullptr) {
				stbi_image_free(m_data[i]);
			}
		}

	}
}