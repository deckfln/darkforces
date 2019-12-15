#pragma once

#include "glFrameBuffer.h"
#include "textures/glDepthTexture.h"

class glDepthMap: public glFrameBuffer
{
protected:
	glTexture *m_depth;

public:
	glDepthMap(int width, int height);
	void clear(void);
	glTexture *getDepthTexture(void);
	~glDepthMap();
};
