#include <iostream>
#include "glTexture.h"
#include "include/stb_image.h"
#include "glad/glad.h"

int glTexture::currentTextureUnit = 0;

glTexture::glTexture(std::string _uniform, std::string file)
{
	uniform = _uniform;
	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
	glBindTexture(GL_TEXTURE_2D, id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLuint pixels;

		switch (nrChannels) {
		case 3: pixels = GL_RGB; break;
		case 4: pixels = GL_RGBA; break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, pixels, width, height, 0, pixels, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void glTexture::bind(glProgram &program)
{
	textureUnit = currentTextureUnit;

	glActiveTexture(GL_TEXTURE0 + currentTextureUnit);
	glBindTexture(GL_TEXTURE_2D, id);
	currentTextureUnit++;

	program.set_uniform(uniform, textureUnit);
}

GLint glTexture::get_textureUnit(void)
{
	return textureUnit;
}

glTexture::~glTexture()
{
}
