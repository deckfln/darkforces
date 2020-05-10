#include "dfSprite.h"

#include "../dfModel/dfWAX.h"

dfSprite::dfSprite(dfWAX* wax, float x, float y, float z):
	dfObject(wax, x, y, z)
{
}

/**
 * Stack up logics
 */
void dfSprite::logic(int logic)
{
	m_logics |= logic;
}

void dfSprite::height(float h)
{
	m_height = h;
}

dfSprite::~dfSprite()
{
}
