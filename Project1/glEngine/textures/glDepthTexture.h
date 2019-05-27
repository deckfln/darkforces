#pragma once

#include "../glTexture.h"

class glDepthTexture : public glTexture
{
public:
	glDepthTexture(int width, int height);
	~glDepthTexture();
};