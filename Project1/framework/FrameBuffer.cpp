#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int _width, int _height):
	width(_width),
	height(_height)
{
	frameBuffer = new glFrameBuffer();
	frameBuffer->bind(GL_FRAMEBUFFER);

	color = new glTexture(_width, _height, GL_RGBA);
	frameBuffer->bindTexture(color, GL_COLOR_ATTACHMENT0);

	depth_stencil = new glRenderBuffer(_width, _height);

	frameBuffer->bindDepth(depth_stencil);

	frameBuffer->unbind();
}

void FrameBuffer::bind(void)
{
	glViewport(0, 0, width, height);
	frameBuffer->bind();
}
glTexture *FrameBuffer::get_colorBuffer(void)
{
	return color;
}

void FrameBuffer::unbind(void)
{
	frameBuffer->unbind();
}

FrameBuffer::~FrameBuffer()
{
	delete frameBuffer;
	delete color;
	delete depth_stencil;
}