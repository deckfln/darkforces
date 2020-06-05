#include <iostream>
#include <stack>
#include "glTexture.h"
#include "../include/stb_image.h"
#include "../glad/glad.h"

int glTexture::c_currentTextureUnit = 0;
int glTexture::c_max_TextureUnits = -1;
glTexture** glTexture::c_textureUnitBinding = nullptr;

static std::stack<int> stack;

glTexture::glTexture()
{

}

/**
 * Create an empty texture
 */
glTexture::glTexture(int width, int height, int format, int channels, int filter)
{
	glGenTextures(1, &id);

	if (c_max_TextureUnits < 0) {
		initTextureUnits();
	}
	glActiveTexture(GL_TEXTURE0 + c_max_TextureUnits);	// new texture are alway bound to the last texture
	glBindTexture(GL_TEXTURE_2D, id);

	int type = -1;

	if (channels < 0) {
		channels = format;
	}
	else {
		switch (channels) {
		case 1: channels = GL_RED; break;
		case 2: channels = GL_RG; break;
		case 3: channels = GL_RGB; break;
		case 4: channels = GL_RGBA; break;
		}
	}

	switch (format) {
	case GL_RED:
	case GL_RG:
	case GL_RGB:
	case GL_RGBA:
		type = GL_UNSIGNED_BYTE;
		break;
	case GL_RGB16F:
		type = GL_FLOAT;
		break;
	case GL_DEPTH_COMPONENT:
		type = GL_FLOAT;
		filter = GL_NEAREST;
		break;
	}
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, channels, type, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Create the GL texture
 */
void glTexture::init(void) 
{
	glGenTextures(1, &id);
	if (c_max_TextureUnits < 0) {
		initTextureUnits();
	}
	glActiveTexture(GL_TEXTURE0 + c_max_TextureUnits);	// new texture are alway bound to the last texture
	glBindTexture(GL_TEXTURE_2D, id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filter);

	GLuint pixels = GL_RGB;

	switch (m_nrChannels) {
	case 1: pixels = GL_RED; break;
	case 2: pixels = GL_RG; break;
	case 3: pixels = GL_RGB; break;
	case 4: pixels = GL_RGBA; break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, pixels, m_width, m_height, 0, pixels, GL_UNSIGNED_BYTE, m_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Create a texture from raw data
 */
glTexture::glTexture(unsigned char *data, int width, int height, int channels, int filter):
	m_data(data),
	m_width(width),
	m_height(height),
	m_nrChannels(channels),
	m_filter(filter)
{
	init();
}

/**
 * Create from a framework texture
 */
glTexture::glTexture(fwTexture* texture)
{
	m_filter = texture->filter();
	// load and generate the texture
	m_data = texture->get_info(&m_width, &m_height, &m_nrChannels);

	init();
}

GLuint glTexture::getID(void)
{
	return id;
}

void glTexture::PushTextureUnit(void)
{
	stack.push(c_currentTextureUnit);
}

void glTexture::PopTextureUnit(void)
{
	c_currentTextureUnit = stack.top();
	stack.pop();
}

GLint glTexture::bind(void)
{
	if (c_max_TextureUnits < 0) {
		initTextureUnits();
	}

	// check if the texture is already bound to a textureUnit
	if (textureUnit == -1) {
		int unit = -2;
		for (auto i = 0; i < c_max_TextureUnits; i++) {
			if (c_textureUnitBinding[i] == nullptr) {
				// reached the end of bound texures. the texture is not yet bound
				c_textureUnitBinding[i] = this;

				glActiveTexture(GL_TEXTURE0 + i);  //reserve texture0 for new textures
				_bind(id);

				unit = textureUnit = i;

				break;
			}
		}

		if (unit == -2) {
			// the texture is not loaded but there is no space left to load it
			printf("glTexture::bind textureUnitBinding full not handled\n");
			exit(-1);
		}
	}
	else {
		//TODO: optimize texture units utization accross programs 
		//TODO: look at glBindTextureS
		//glActiveTexture(GL_TEXTURE0 + textureUnit);
		//glBindTexture(GL_TEXTURE_2D, id);
	}

	return textureUnit;
}

GLint glTexture::get_textureUnit(void)
{
	return textureUnit;
}

glTexture::~glTexture()
{
	glDeleteTextures(1, &id);
}
