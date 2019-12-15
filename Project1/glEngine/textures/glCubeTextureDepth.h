#pragma once

#include "../glCubeTexture.h"

class glCubeTextureDepth : public glCubeTexture
{
public:
	glCubeTextureDepth(int width, int height);
	~glCubeTextureDepth();
};