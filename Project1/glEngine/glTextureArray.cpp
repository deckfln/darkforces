#include <iostream>
#include <stack>
#include "glTextureArray.h"
#include "../include/stb_image.h"

glTextureArray::glTextureArray()
{

}

glTextureArray::glTextureArray(int width, int height, int format, int channels, int filter)
{

}

glTextureArray::glTextureArray(fwTextures* textures) 
{
	glGenTextures(1, &id);
	if (c_max_TextureUnits < 0) {
		initTextureUnits();
	}
	glActiveTexture(GL_TEXTURE0 + c_max_TextureUnits);	// new texture are alway bound to the last texture
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	

	// load and generate the texture
	int width, height, nrChannels, nrLayers;
	unsigned char *data = textures->get_info(width, height, nrChannels, nrLayers);
	GLuint pixels = GL_RGB;

	switch (nrChannels) {
	case 1: pixels = GL_RED; break;
	case 2: pixels = GL_RG; break;
	case 3: pixels = GL_RGB; break;
	case 4: pixels = GL_RGBA; break;
	}

	// reserve opengl Memory
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, pixels, width, height, nrLayers, 0, pixels, GL_UNSIGNED_BYTE, NULL);
	
	//Specify i-essim image
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
		0,                     //Mipmap number
		0, 0, 0,               //xoffset, yoffset, zoffset
		width, height, nrLayers,               //width, height, depth
		pixels,                //format
		GL_UNSIGNED_BYTE,      //type
		data);                //pointer to data

	// glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, pixels, width, height, nrLayers, 0, pixels, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

glTextureArray::~glTextureArray()
{

}