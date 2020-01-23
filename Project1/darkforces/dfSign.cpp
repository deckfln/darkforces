#include "dfSign.h"

#include <string>
#include "dfBitmap.h"
#include "dfsuperSector.h"

dfSign::dfSign(dfSuperSector* ssector, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<float>& textureIDs, dfBitmap *bitmap, int start, int size, dfSector* sector, dfWall* wall) :
	m_supersector(ssector),
	m_vertices(vertices),
	m_uvs(uvs),
	m_textureIDs(textureIDs),
	m_start(start),
	m_size(size),
	m_bitmap(bitmap)
{
	m_name = sector->m_name + "(" + std::to_string(wall->m_id) + ")";
}

/**
 * change the display of the sign
 */
void dfSign::setStatus(int status)
{
	dfBitmapImage* image = m_bitmap->getImage(status);	// extract the target image
	float textureID = (float)image->m_textureID;				// index of the image in the atlas map

	// update the vertices with the new textureID
	for (int i = m_start; i < m_start + m_size; i++) {
		m_textureIDs[i] = textureID;
	}

	// push the changes
	m_supersector->updateGeometryTextures(m_start, m_size);
	// printf("void dfSign::setStatus %d\n", status);
}

