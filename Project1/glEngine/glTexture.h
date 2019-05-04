#pragma once
#include "glad/glad.h"
#include "glEngine/glProgram.h"
#include "framework/Texture.h"

class glTexture
{
	static int currentTextureUnit;

	std::string uniform;
	GLuint id;
	GLuint textureUnit;
public:
	glTexture(std::string uniform, std::string file);
	glTexture(std::string uniform, Texture *texture);
	void bind(void);
	static void resetTextureUnit(void) { currentTextureUnit = 0; };
	GLint get_textureUnit(void);
	~glTexture();
};
