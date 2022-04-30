#pragma once

#include "../glDepthMap.h"
#include "../textures/glCubeTextureDepth.h"

class glDepthCubeMap : public glDepthMap
{
public:
	glDepthCubeMap(int width, int height);
};
