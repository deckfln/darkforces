#pragma once

#include <string>
#include <glm/vec2.hpp>

// data for sprite model
struct SpriteModel {
	glm::vec2 size;		// sprite size in pixel
	glm::vec2 insert;	// offset of center point
	glm::vec2 world;
	glm::vec2 textureID;
};

class dfModel
{
protected:
	std::string m_name;
public:
	dfModel(std::string& name);
	bool named(std::string& name);
	std::string& name(void) { return m_name; };
	virtual int textureID(int state, int frame) { return 0; };
	virtual void spriteModel(SpriteModel *sm) {};
	virtual int framerate(int state) { return 0; }
	virtual int nextFrame(int state, unsigned int frame) { return 0; };
};