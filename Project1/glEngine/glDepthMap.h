#pragma once

#include "glFrameBuffer.h"

class glDepthMap: public glFrameBuffer
{
	glTexture *depth;

public:
	glDepthMap(int width, int height);
	void clear(void);
	glTexture *getDepthTexture(void);
	~glDepthMap();
};
