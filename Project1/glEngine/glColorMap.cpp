#include "glColorMap.h"

static unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

glColorMap::glColorMap(int _width, int _height) : 
	glFrameBuffer(_width, _height)
{
}

glColorMap::glColorMap(int _width, int _height, int _colorBuffers, int mask, glTexture* extra) :
	glFrameBuffer(_width, _height),
	m_allocatedBuffers(_colorBuffers)
{
	bind();

	m_totalBuffers = m_allocatedBuffers;
	if (extra != nullptr) {
		m_totalBuffers++;
	}

	colors = new glTexture *[m_totalBuffers];

	// need to read back in next stage => use a TextureBuffer
	// 0 -> color buffer
	for (int i = 0; i < m_allocatedBuffers; i++) {
		colors[i] = new glTexture(m_size.x, m_size.y, GL_RGBA);
	}

	if (extra != nullptr) {
		colors[m_totalBuffers - 1] = extra;
	}

	for (int i = 0; i < m_totalBuffers; i++) {
		bindTexture(colors[i], GL_COLOR_ATTACHMENT0 + i);
	}

	glDrawBuffers(m_totalBuffers, attachments);

	// don't need to read the content of the depth & stencil buffer later => use a RenderBuffer
	if (mask == 3) {
		depth_stencil = new glRenderBuffer(m_size.x, m_size.y);
		bindDepth(depth_stencil);
	}

	unbind();
}

void glColorMap::clear(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	if (m_mask & GL_COLORMAP_DEPTH) {
		enable(GL_DEPTH_TEST);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if (m_mask & GL_COLORMAP_DEPTH) {
		restore(GL_DEPTH_TEST);
	}
}

void glColorMap::enable(int mask)
{
	glGetBooleanv(mask, &m_previous_state[mask]);
	if (! &m_previous_state[mask]) {
		glEnable(mask);
	}
}

void glColorMap::disable(int mask)
{
	glGetBooleanv(mask, &m_previous_state[mask]);
	if (&m_previous_state[mask]) {
		glDisable(mask);
	}
}

void glColorMap::restore(int mask)
{
	if (m_previous_state[mask]) {
		glEnable(mask);
	}
	else {
		glDisable(mask);
	}
}

glRenderBuffer* glColorMap::get_stencil(void)
{
	return depth_stencil;
}

glTexture *glColorMap::getColorTexture(int index)
{
	if (index >= 0 && index < m_allocatedBuffers)
		return colors[index];

	return nullptr;
}

void glColorMap::bindColors(int colorBuffers)
{
	if (m_totalBuffers == 1)
		return;

	if (colorBuffers == -1) {
		// restore previous value
		glDrawBuffers(m_prev_colorBuffers, attachments);
	}
	else {
		m_prev_colorBuffers = m_allocatedBuffers;
		glDrawBuffers(colorBuffers, attachments);
	}
}

glColorMap::~glColorMap()
{
	for (auto i = 0; i < m_allocatedBuffers; i++) {
		delete colors[i];
	}
	delete colors;

	if (depth_stencil != nullptr) {
		delete depth_stencil;
	}
}