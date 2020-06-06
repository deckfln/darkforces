#include "dfSign.h"

#include <string>
#include "dfBitmap.h"
#include "dfsuperSector.h"

dfSign::dfSign(dfMesh *mesh, dfSector* sector, dfWall* wall, float z, float z1) :
	dfMesh(mesh)
{
	m_name = sector->m_name + "(" + std::to_string(wall->m_id) + ")";
	buildGeometry(sector, wall, z, z1);
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
		(*m_pTextureIDs)[i] = textureID;
	}

	// push the changes
	m_parent->updateGeometryTextures(m_start, m_size);
	// printf("void dfSign::setStatus %d\n", status);
}

void dfSign::rebuildAABB(void)
{
	m_boundingBox.reset();
	for (int i = m_start; i < m_start + m_size; i++) {
		m_boundingBox.extend((*m_pVertices)[i]);
	}
}

void dfSign::buildGeometry(dfSector* sector, dfWall* wall, float z, float z1)
{
	// resize the opengl buffers
	int p = resize(6);

	m_start = p;
	m_size = 6;

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	float bitmapID = wall->m_tex[DFWALL_TEXTURE_SIGN].r;
	m_bitmap = m_bitmaps[(int)bitmapID];

	dfBitmapImage* image = m_bitmap->getImage();

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = (float)image->m_width;
	float ypixel = (float)image->m_height;

	glm::vec2 segment = glm::normalize(glm::vec2(x1 - x, y1 - y));
	glm::vec2 start = sector->m_vertices[wall->m_left] + segment * (wall->m_tex[DFWALL_TEXTURE_SIGN].g - wall->m_tex[DFWALL_TEXTURE_MID].g);
	glm::vec2 end = sector->m_vertices[wall->m_left] + segment * (wall->m_tex[DFWALL_TEXTURE_SIGN].g - wall->m_tex[DFWALL_TEXTURE_MID].g + xpixel / 8.0f);

	// wall normals
	glm::vec3 normal = glm::normalize(glm::vec3(-segment.y, segment.x, 0));	//  and (dy, -dx).

	// create a copy of the wall and shrink to the size and position of the sign
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	glm::vec3 sign_p = glm::vec3(
		start.x,
		start.y,
		//z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b + wall->m_tex[DFWALL_TEXTURE_MID].b)
		z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b)
		);
	glm::vec3 sign_p1 = glm::vec3(
		end.x,
		end.y,
		// z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b + wall->m_tex[DFWALL_TEXTURE_MID].b) + ypixel / 8.0f
		z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b) + ypixel / 8.0f
		);

	// move the the wall along the normal
	sign_p += normal / 10.0f;
	sign_p1 += normal / 10.0f;

	// Handle request to flip the texture
	bool flipTexture = wall->flag1(dfWallFlag::FLIP_TEXTURE_HORIZONTALLY);

	glm::vec2 offset(0, 0);
	glm::vec2 size(1, 1);

	updateRectangle(p, 
		sign_p, 
		sign_p1, 
		offset, 
		size, 
		image->m_textureID, 
		sector->m_ambient);
}
