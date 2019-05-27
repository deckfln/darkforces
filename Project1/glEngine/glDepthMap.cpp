#include "glDepthMap.h"

glDepthMap::glDepthMap(int _width, int _height) :
	glFrameBuffer(_width, _height)
{
	bind();
	// need to read back in next stage => use a TextureBuffer
	depth = new glTexture(_width, _height, GL_DEPTH_COMPONENT);
	bindTexture(depth, GL_DEPTH_ATTACHMENT);
	glDrawBuffer(GL_NONE);	// no color buffer
	glReadBuffer(GL_NONE);
	unbind();
}

void glDepthMap::clear(void)
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

glTexture *glDepthMap::getDepthTexture(void)
{
	return depth;
}

glDepthMap::~glDepthMap()
{

}
