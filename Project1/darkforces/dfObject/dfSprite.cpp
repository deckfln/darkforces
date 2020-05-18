#include "dfSprite.h"

#include "../dfModel/dfWAX.h"

dfSprite::dfSprite(dfWAX* wax, float x, float y, float z):
	dfObject(wax, x, y, z)
{
}

void dfSprite::height(float h)
{
	m_height = h;
}

dfSprite::~dfSprite()
{
}
