#pragma once

#include "../dfObject.h"

class dfWAX;

class dfSprite: public dfObject
{
	float m_height = 0;
	int m_state = 0;	// current state of the sprite
	int m_frame = 0;	// frame position in the animation

public:
	dfSprite(dfWAX* wax, float x, float y, float z);
	void height(float h);
	~dfSprite();
};