#include "dfSprite.h"

#include "../dfWAX.h"

dfSprite::dfSprite(dfWAX* wax, float x, float y, float z):
	m_wax(wax),
	m_x(x),
	m_y(y),
	m_z(z)
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
