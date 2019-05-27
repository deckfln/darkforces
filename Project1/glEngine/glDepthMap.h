#pragma once

#include "glFrameBuffer.h"
#include "textures/glDepthTexture.h"

class glDepthMap: public glFrameBuffer
{
	glDepthTexture *depth;

public:
	glDepthMap(int width, int height);
	void clear(void);
	glDepthTexture *getDepthTexture(void);
	~glDepthMap();
};
