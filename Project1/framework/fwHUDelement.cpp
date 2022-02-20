#include "fwHUDelement.h"

#include "fwTexture.h"
#include "../glEngine/glProgram.h"
#include "fwMaterial.h"
#include "fwGeometry.h"
#include "fwFlatPanel.h"

/**
 * if LOCKED_Y, width is extended based on height
 * if LOCKED_X, height is computed based on width
 * if UNLOCKED, width & height are mandaotry
 */
fwHUDelement::fwHUDelement(const std::string& name, Position position, fwHUDelementSizeLock lock, float width, float height, fwTexture *texture, fwFlatPanel *panel):
	m_name(name),
	m_position(position),
	m_sizeLock(lock),
	m_texture(texture),
	m_panel(panel)
{
	m_onscreen.x = width;
	m_onscreen.y = height;

	switch(position) {
	case Position::BOTTOM_LEFT:
		m_onscreen.z = (2.0f * width) / 2.0f - 1.0f;
		m_onscreen.w = (2.0f * height) / 2.0f - 1.0f;
		break;
	case Position::BOTTOM_RIGHT:
		m_onscreen.z = 1.0f - (2.0f * width) / 2.0f;
		m_onscreen.w = (2.0f * height) / 2.0f - 1.0f;
		break;
	case Position::BOTTOM_CENTER:
		m_onscreen.z = 0;
		m_onscreen.w = (2.0f * height) / 2.0f - 1.0f;
		break;
	}
}

/**
 * force the X position (between -1. and 1.0)
 */
void fwHUDelement::position(float x, float y)
{
	m_onscreen.z = x;
	m_onscreen.w = y;
}

/**
 * force the size in glspace (between -1. and 1.0)
 */
void fwHUDelement::size(float w, float h)
{
	m_onscreen.x = w;
	m_onscreen.y = h;
}

/**
 * change the current texture
 */
void fwHUDelement::texture(fwTexture* texture)
{
	m_texture = texture;
}

/**
 * dedicated uniforms for that element
 */
void fwHUDelement::addUniform(fwUniform* uniform)
{
	m_uniforms.push_back(uniform);
}

void fwHUDelement::draw(fwFlatPanel *panel)
{
	if (m_panel) {
		// dedicated panel
		m_panel->set("image", m_texture);
		m_panel->set("onscreen", &m_onscreen);
		m_panel->draw();
	}
	else {
		// use the default panel
		panel->set("image", m_texture);
		panel->set("onscreen", &m_onscreen);
		panel->draw();
	}
}

fwHUDelement::~fwHUDelement()
{
}
