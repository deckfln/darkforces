#pragma once
#include "glad/glad.h"
#include "glEngine/glProgram.h"

class glTexture
{
	static int currentTextureUnit;

	std::string uniform;
	GLuint id;
	GLuint textureUnit;
public:
	glTexture(std::string uniform, std::string file);
	void bind(glProgram &program);
	static void resetTextureUnit(void) { currentTextureUnit = 0; };
	GLint get_textureUnit(void);
	~glTexture();
};
