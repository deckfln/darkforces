#pragma once
/*************************************
 *
 * Draw circular skyline using a single texture
 *
 *************************************/

#include <string>
#include <glm/vec2.hpp>

#include "../fwBackground.h"

class glTexture;

class fwSkyline : public fwBackground
{
	int m_height = 0;
	glm::vec2 m_repeat = glm::vec2(1, 1);
	glTexture* m_texture = nullptr;

public:
	fwSkyline(unsigned char* data, int width, int height, int channels, int repeat_x=1, int repeat_y=1);
};