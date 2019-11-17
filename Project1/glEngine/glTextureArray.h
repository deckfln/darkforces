#pragma once
#include "../glad/glad.h"
#include "../framework/fwTextures.h"
#include "glTexture.h"

class glTextureArray : glTexture
{
public:
	glTextureArray();
	glTextureArray(int width, int height, int format, int channels = -1, int filter = GL_LINEAR);
	glTextureArray(fwTextures* textures);
	~glTextureArray();
};