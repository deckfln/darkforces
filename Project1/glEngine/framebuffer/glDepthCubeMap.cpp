#include "glDepthCubeMap.h"

glDepthCubeMap::glDepthCubeMap(int width, int height) :
	glDepthMap(width, height)
{
	delete m_depth;

	bind();
	// need to read back in next stage => use a TextureBuffer
	m_depth = new glCubeTextureDepth(width, height);
	bindTexture(m_depth, GL_DEPTH_ATTACHMENT);
	glDrawBuffer(GL_NONE);	// no color buffer
	glReadBuffer(GL_NONE);
	unbind();
}
