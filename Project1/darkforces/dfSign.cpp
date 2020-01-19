#include "dfSign.h"

#include "dfWall.h"

dfSign::dfSign(dfWall* parent, glm::vec3* vertices, glm::vec2* uvs, float* textureID) :
	m_parent(parent),
	m_vertices(vertices),
	m_uvs(uvs),
	m_textureID(textureID)
{
}

/**
 * change the display of the sign
 */
void dfSign::setStatus(int status)
{
	printf("void dfSign::setStatus %d\n", status);
}
