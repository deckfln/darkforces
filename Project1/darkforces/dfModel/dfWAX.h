#pragma once
#include <string>
#include <vector>
#include <map>

#include <glm/vec2.hpp>

#include "../../framework/fwAABBox.h"
#include "../dfModel.h"

class dfFileSystem;
class dfFrame;
class dfPalette;
class dfBitmapImage;

enum dfWaxMode {
	DF_WAX_ENEMY,
	DF_WAX_SCENERY,
	DF_WAX_BARREL
};

struct dfWaxAnimation {
	int m_nbframes = 0;
	glm::ivec2 m_size = glm::vec2(0);	// size in pixels of the animation (maximum size of each frame)
	glm::ivec2 m_insert = glm::vec2(0);	// offset of object (maximum size of each frame/angle)
	std::vector<dfFrame *> frames;
};

struct dfWaxState {
	int m_Wwidth = 0;	// World Width
	int m_Wheight = 0;
	int m_FrameRate = 0;
	std::vector<dfWaxAnimation *> animations;

	glm::ivec2 m_size = glm::vec2(0);	// size in pixels of the state (maximum size of each frame/angle)
	glm::ivec2 m_insert = glm::vec2(0);	// offset of object (maximum size of each frame/angle)
	fwAABBox m_cylinder;				// AABB boundingbox of that specific state
};

// compile time class
extern const uint32_t g_wax_class;

class dfWAX: public dfModel
{
	void* m_data = nullptr;

	int m_width = 0;		// size of the sprite (max of all frames)
	int m_height = 0;
	int m_insertX = 0;		// Insertion point, X coordinate
	int m_insertY = 0;		// Insertion point, Y coordinate
	long m_Wwidth = 0;		// world width factor
	long m_Wheight = 0;		// world height factor

	int m_nbStates = 0;
	std::vector<dfWaxState *> m_states;

	std::map<int, dfWaxAnimation*> m_animations;
	std::map<int, dfFrame *> m_frames;

public:
	dfWAX(dfFileSystem* fs, dfPalette *palette, const std::string& name);
	void getFrames(std::vector<dfBitmapImage*>& m_frames);
	virtual int textureID(int state, int frame);
	virtual void spriteModel(GLmodel& model, int id);
	virtual int framerate(int state);
	virtual int nextFrame(int state, unsigned int frame);
	int insertX(void) { return m_insertX; };
	int insertY(void) { return m_insertY; };
	const fwAABBox& bounding(int state);
	~dfWAX();
};