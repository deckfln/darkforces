#include <iostream>
#include <stack>
#include "glTexture.h"
#include "../include/stb_image.h"
#include "../glad/glad.h"

int glTexture::currentTextureUnit = 0;

static int max_TextureUnits = -1;

static std::stack<int> stack;
static glTexture **textureUnitBinding;

static void _initTextureUnits(void)
{
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_TextureUnits);
	max_TextureUnits -= 2;	// reserve texture0 for new textures
	textureUnitBinding = new (glTexture * [max_TextureUnits]);
	for (auto i = 0; i < max_TextureUnits; i++) {
		textureUnitBinding[i] = nullptr;
	}
}

glTexture::glTexture()
{

}

glTexture::glTexture(int width, int height, int format, int channels, int filter)
{
	glGenTextures(1, &id);

	if (max_TextureUnits < 0) {
		_initTextureUnits();
	}
	glActiveTexture(GL_TEXTURE0 + max_TextureUnits);	// new texture are alway bound to the last texture
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

glTexture::glTexture(fwTexture* texture)
{
	glGenTextures(1, &id);
	if (max_TextureUnits < 0) {
		_initTextureUnits();
	}
	glActiveTexture(GL_TEXTURE0 + max_TextureUnits);	// new texture are alway bound to the last texture
	glBindTexture(GL_TEXTURE_2D, id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = texture->get_info(&width, &height, &nrChannels);
	GLuint pixels = GL_RGB;

	switch (nrChannels) {
	case 1: pixels = GL_RED; break;
	case 2: pixels = GL_RG; break;
	case 3: pixels = GL_RGB; break;
	case 4: pixels = GL_RGBA; break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, pixels, width, height, 0, pixels, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint glTexture::getID(void)
{
	return id;
}

void glTexture::PushTextureUnit(void)
{
	stack.push(currentTextureUnit);
}

void glTexture::PopTextureUnit(void)
{
	currentTextureUnit = stack.top();
	stack.pop();
}

GLint glTexture::bind(void)
{
	if (max_TextureUnits < 0) {
		_initTextureUnits();
	}

	// check if the texture is already bound to a textureUnit
	if (textureUnit == -1) {
		int unit = -2;
		for (auto i = 0; i < max_TextureUnits; i++) {
			if (textureUnitBinding[i] == nullptr) {
				// reached the end of bound texures. the texture is not yet bound
				textureUnitBinding[i] = this;

				glActiveTexture(GL_TEXTURE0 + i);  //reserve texture0 for new textures
				glBindTexture(GL_TEXTURE_2D, id);

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
