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
		data[i] = stbi_load(files[i].c_str(), &width, &height, &nrChannels, 0);
		GLuint glformat = GL_RGB;

		switch (nrChannels) {
		case 3: glformat = GL_RGB; break;
		case 4: glformat = GL_RGBA; break;
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, glformat, width, height, 0, glformat, GL_UNSIGNED_BYTE, data[i]
		);
	}
}

void *glCubeTexture::get_info(int *_width, int *_height, int *_nrChannels)
{
	*_width = width;
	*_height = height;
	*_nrChannels = nrChannels;
	return data;
}

glCubeTexture::~glCubeTexture()
{
	for (int i = 0; i < 6; i++) {
		if (data[i] != nullptr) {
			stbi_image_free(data[i]);
		}
	}
}