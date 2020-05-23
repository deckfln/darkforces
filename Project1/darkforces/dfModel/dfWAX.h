#pragma once
#include <string>
#include <vector>
#include <map>

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
	std::vector<dfFrame *> frames;
};

struct dfWaxAngles {
	int m_Wwidth = 0;	// World Width
	int m_Wheight = 0;
	int m_FrameRate = 0;
	std::vector<dfWaxAnimation *> animations;
};

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
	std::vector<dfWaxAngles *> m_states;

	std::map<int, dfWaxAnimation*> m_animations;
	std::map<int, dfFrame *> m_frames;

public:
	dfWAX(dfFileSystem* fs, dfPalette *palette, std::string& name);
	void getFrames(std::vector<dfBitmapImage*>& m_frames);
	virtual int textureID(int state, int frame);
	virtual void spriteModel(GLmodel& model, int id);
	virtual int framerate(int state);
	virtual int nextFrame(int state, unsigned int frame);
	int insertX(void) { return m_insertX; };
	int insertY(void) { return m_insertY; };
	~dfWAX();
};