#pragma once

#include <string>
#include <glm/vec2.hpp>
#include <vector>
#include "../glad/glad.h"

// data for sprite model

struct Angle {
	unsigned short textureID[32];
};

struct State {
	struct Angle angles[32];
};

struct SpriteModel {
	glm::vec2 size;		// sprite size in pixel
	glm::vec2 insert;	// offset of center point
	glm::vec2 world;
	glm::ivec2 states;	// index of the start of the states
};

struct GLmodel {
	SpriteModel models[32];

	GLuint stateTable[128];
	GLuint angleTable[2048];
	GLuint frameTable[512];

	int stIndex = 0, atIndex = 0, ftIndex = 0;
};

class dfModel
{
protected:
	std::string m_name;
	int m_id = 0;

public:
	dfModel(std::string& name);
	bool named(std::string& name);
	std::string& name(void) { return m_name; };
	int id(void) { return m_id; };
	virtual int textureID(int state, int frame) { return 0; };
	virtual void spriteModel(GLmodel& model, int id) {};
	virtual int framerate(int state) { return 0; }
	virtual int nextFrame(int state, unsigned int frame) { return 0; };
};