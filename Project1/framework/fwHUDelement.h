#pragma once

#include <string>
#include <glm/vec4.hpp>

class fwTexture;
class fwFlatPanel;

enum class fwHUDElementPosition {
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT,
	TOP_LET,
	TOP_CENTER,
	TOP_RIGHT,
	CENTER_LEFT,
	CENTER_CENTER,
	CENTER_RIGHT
};

enum class fwHUDelementSizeLock {
	UNLOCKED,
	LOCKED_Y,
	LOCKED_X
};

class fwHUDelement
{
	std::string m_name;
	fwHUDElementPosition m_position = fwHUDElementPosition::BOTTOM_CENTER;
	fwHUDelementSizeLock m_sizeLock = fwHUDelementSizeLock::UNLOCKED;
	glm::vec4 m_onscreen = glm::vec4(0);
	fwTexture* m_texture = nullptr;

public:
	fwHUDelement(const std::string& name, fwHUDElementPosition position, fwHUDelementSizeLock lock, float width, float height, fwTexture *texture);
	void draw(fwFlatPanel* panel);
	~fwHUDelement();
};
