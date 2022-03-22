#pragma once

#include <string>
#include <vector>
#include <glm/vec4.hpp>

class fwTexture;
class fwFlatPanel;
class fwUniform;

enum class fwHUDElementPosition {
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT,
	TOP_LET,
	TOP_CENTER,
	TOP_RIGHT,
	CENTER_LEFT,
	CENTER_CENTER,
	CENTER_RIGHT,
	BOTTOM_X,
	TOP_Y
};

enum class fwHUDelementSizeLock {
	UNLOCKED,
	LOCKED_Y,
	LOCKED_X
};

class fwHUDelement
{
public:
	enum class Position {
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

	fwHUDelement(const std::string& name);
	fwHUDelement(const std::string& name, Position position, fwHUDelementSizeLock lock, float width, float height, fwTexture* texture, fwFlatPanel* panel = nullptr);
	void position(float x, float y);		// force the X position (between -1. and 1.0)
	void size(float w, float h);			// force the size in glspace (between -1. and 1.0)
	void texture(fwTexture* texture);		// change the current texture
	void addUniform(fwUniform* uniform);	// dedicated uniforms for that element
	inline void visible(bool b) { m_visible = b; };
	inline bool visible(void) { return m_visible; };
	virtual void draw(fwFlatPanel* panel);
	~fwHUDelement();

protected:
	std::string m_name;
	Position m_position = Position::BOTTOM_CENTER;
	fwHUDelementSizeLock m_sizeLock = fwHUDelementSizeLock::UNLOCKED;
	glm::vec4 m_onscreen = glm::vec4(0);
	fwTexture* m_texture = nullptr;
	std::vector<fwUniform*> m_uniforms;
	fwFlatPanel* m_panel = nullptr;
	bool m_visible = true;
};
