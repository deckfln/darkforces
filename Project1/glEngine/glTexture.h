#pragma once
#include "../glad/glad.h"
#include "../framework/fwTexture.h"

class glTexture
{
protected:
	static int c_currentTextureUnit;
	static int c_max_TextureUnits;
	static glTexture** c_textureUnitBinding;

	GLuint id = -1;
	GLuint textureUnit = -1;

	static void initTextureUnits(void)
	{
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &c_max_TextureUnits);
		c_max_TextureUnits -= 2;	// reserve texture0 for new textures
		c_textureUnitBinding = new (glTexture * [c_max_TextureUnits]);
		for (auto i = 0; i < c_max_TextureUnits; i++) {
			c_textureUnitBinding[i] = nullptr;
		}
	}


public:
	glTexture();
	glTexture(int width, int height, int format, int channels = -1, int filter=GL_LINEAR);
	glTexture(fwTexture *texture);
	GLuint getID(void);
	GLint bind(void);
	static void PushTextureUnit(void);
	static void PopTextureUnit(void);
	GLint get_textureUnit(void);
	~glTexture();
};
