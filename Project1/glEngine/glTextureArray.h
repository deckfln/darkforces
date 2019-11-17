#pragma once
#include "../glad/glad.h"
#include "../framework/fwTextures.h"
#include "glTexture.h"

class glTextureArray : glTexture
{
protected:
	virtual void _bind(GLint id) { glBindTexture(GL_TEXTURE_2D_ARRAY, id); }

public:
	glTextureArray();
	glTextureArray(int width, int height, int format, int channels = -1, int filter = GL_LINEAR);
	glTextureArray(fwTextures* textures);
	virtual void bind() {}
	~glTextureArray();
};