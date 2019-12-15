#include "glCubeTexture.h"

#include "../glad/glad.h"
#include "../include/stb_image.h"


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
}

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
	for (int i = 0; i < 6; i++) {
		if (m_data[i] != nullptr) {
			stbi_image_free(m_data[i]);
		}
	}
}