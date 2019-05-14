#pragma once
#include "glad/glad.h"
#include "glEngine/glProgram.h"
#include "framework/Texture.h"

class glTexture
{
protected:
	static int currentTextureUnit;

	GLuint id = -1;
	GLuint textureUnit = 0;
public:
	glTexture();
	glTexture(int width, int height, int format);
	glTexture(Texture *texture);
	GLuint getID(void);
	void bind(void);
	static void resetTextureUnit(void) { currentTextureUnit = 0; };
	GLint get_textureUnit(void);
	~glTexture();
};
