#pragma once

#include "../dfObject.h"

class dfWAX;

class dfSprite: public dfObject
{
	float m_x, m_y, m_z;
	dfWAX* m_wax;
	int m_logics = 0;
	float m_height = 0;

public:
	dfSprite(dfWAX* wax, float x, float y, float z);
	void logic(int logic);
	void height(float h);
	~dfSprite();
};