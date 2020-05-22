#pragma once

#include "../dfObject.h"

class dfWAX;

class dfSprite: public dfObject
{
	int m_state = 0;	// current state of the sprite
	int m_frame = 0;	// frame position in the animation

public:
	dfSprite(dfWAX* wax, float x, float y, float z);
	void height(float h);
	~dfSprite();
};