#include <iostream>
#include "glTexture.h"
#include "include/stb_image.h"
#include "glad/glad.h"

int glTexture::currentTextureUnit = 0;

glTexture::glTexture(int width, int height, int format)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

glTexture::glTexture(Texture *texture)
{
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = texture->get_info(&width, &height, &nrChannels);
	GLuint pixels;

	switch (nrChannels) {
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

void glTexture::bind(void)
{
	textureUnit = currentTextureUnit;

	glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
	glBindTexture(GL_TEXTURE_2D, id);
	currentTextureUnit++;
}

GLint glTexture::get_textureUnit(void)
{
	return textureUnit;
}

glTexture::~glTexture()
{
	glDeleteTextures(1, &id);
}
