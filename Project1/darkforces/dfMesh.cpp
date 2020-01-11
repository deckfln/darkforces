#include "dfMesh.h"

#include "dfSector.h"
#include "dfTexture.h"

dfMesh::dfMesh(fwMaterial* material)
{
	m_material = material;
}

/**
 * create a elevator geometry based on a sector 
 */
void dfMesh::buildGeometry(dfSector* source, float bottom, float top)
{

}

/**
 * Update the vertices of a rectangle
 */
void dfMesh::updateRectangle(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, float textureID)
{
	// TODO move conversion from level space to gl space in a dedicated function
	// first triangle
	m_vertices[p].x = x / 10;
	m_vertices[p].z = y / 10;
	m_vertices[p].y = z / 10;
	m_uvs[p] = glm::vec2(xoffset, yoffset);
	m_textureID[p] = textureID;

	m_vertices[p + 1].x = x1 / 10;
	m_vertices[p + 1].z = y1 / 10;
	m_vertices[p + 1].y = z / 10;
	m_uvs[p + 1] = glm::vec2(width + xoffset, yoffset);
	m_textureID[p + 1] = textureID;

	m_vertices[p + 2].x = x1 / 10;
	m_vertices[p + 2].z = y1 / 10;
	m_vertices[p + 2].y = z1 / 10;
	m_uvs[p + 2] = glm::vec2(width + xoffset, height + yoffset);
	m_textureID[p + 2] = textureID;

	// second triangle
	m_vertices[p + 3].x = x / 10;
	m_vertices[p + 3].z = y / 10;
	m_vertices[p + 3].y = z / 10;
	m_uvs[p + 3] = glm::vec2(xoffset, yoffset);
	m_textureID[p + 3] = textureID;

	m_vertices[p + 4].x = x1 / 10;
	m_vertices[p + 4].z = y1 / 10;
	m_vertices[p + 4].y = z1 / 10;
	m_uvs[p + 4] = glm::vec2(width + xoffset, height + yoffset);
	m_textureID[p + 4] = textureID;

	m_vertices[p + 5].x = x / 10;
	m_vertices[p + 5].z = y / 10;
	m_vertices[p + 5].y = z1 / 10;
	m_uvs[p + 5] = glm::vec2(xoffset, height + yoffset);
	m_textureID[p + 5] = textureID;
}

/**
 * Update the vertices of a rectangle
 */
void dfMesh::updateRectangleAntiClockwise(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, float textureID)
{
	// TODO move conversion from level space to gl space in a dedicated function
	// first triangle
	m_vertices[p].x = x / 10;
	m_vertices[p].z = y / 10;
	m_vertices[p].y = z / 10;
	m_uvs[p] = glm::vec2(xoffset, yoffset);
	m_textureID[p] = textureID;

	m_vertices[p + 2].x = x1 / 10;
	m_vertices[p + 2].z = y1 / 10;
	m_vertices[p + 2].y = z / 10;
	m_uvs[p + 2] = glm::vec2(width + xoffset, yoffset);
	m_textureID[p + 2] = textureID;

	m_vertices[p + 1].x = x1 / 10;
	m_vertices[p + 1].z = y1 / 10;
	m_vertices[p + 1].y = z1 / 10;
	m_uvs[p + 1] = glm::vec2(width + xoffset, height + yoffset);
	m_textureID[p + 1] = textureID;

	// second triangle
	m_vertices[p + 3].x = x / 10;
	m_vertices[p + 3].z = y / 10;
	m_vertices[p + 3].y = z / 10;
	m_uvs[p + 3] = glm::vec2(xoffset, yoffset);
	m_textureID[p + 3] = textureID;

	m_vertices[p + 5].x = x1 / 10;
	m_vertices[p + 5].z = y1 / 10;
	m_vertices[p + 5].y = z1 / 10;
	m_uvs[p + 5] = glm::vec2(width + xoffset, height + yoffset);
	m_textureID[p + 5] = textureID;

	m_vertices[p + 4].x = x / 10;
	m_vertices[p + 4].z = y / 10;
	m_vertices[p + 4].y = z1 / 10;
	m_uvs[p + 4] = glm::vec2(xoffset, height + yoffset);
	m_textureID[p + 4] = textureID;
}

/***
 * create vertices for a rectangle
 */
int dfMesh::addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfTexture*>& textures)
{
	int p = start;

	if (start == -1) {
		// add a new rectangle
		p = m_vertices.size();
		m_vertices.resize(p + 6);
		m_uvs.resize(p + 6);
		m_textureID.resize(p + 6);
	}

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	// deal with the wall texture
	float textureID = wall->m_tex[texture].x;

	dfTexture* dfTexture = textures[(int)textureID];

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = (float)dfTexture->width;
	float ypixel = (float)dfTexture->height;

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	float height = abs(z1 - z) * 8.0f / ypixel;
	float width = length * 8.0f / xpixel;

	// get local texture offset on the wall
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
	float xoffset = (wall->m_tex[texture].y * 8.0f) / xpixel;
	float yoffset = (wall->m_tex[texture].z * 8.0f) / ypixel;

	updateRectangle(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, textureID);

	if (start >= 0) {
		return 6;	// we updated the rectangle, move to the next rectangle
	}

	return 0;	// we added a new rectangle, keep the index at -1
}


/**
 * create a simple opengl Rectangle
 */
void dfMesh::addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, std::vector<dfTexture*>& textures, bool clockwise)
{
	// add a new rectangle
	int p = m_vertices.size();
	m_vertices.resize(p + 6);
	m_uvs.resize(p + 6);
	m_textureID.resize(p + 6);

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	// deal with the wall texture
	float textureID = texture.x;

	dfTexture* dfTexture = textures[(int)textureID];

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = (float)dfTexture->width;
	float ypixel = (float)dfTexture->height;

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	float height = abs(z1 - z) * 8.0f / ypixel;
	float width = length * 8.0f / xpixel;

	// get local texture offset on the wall
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
	float xoffset = (texture.y * 8.0f) / xpixel;
	float yoffset = (texture.z * 8.0f) / ypixel;

	if (clockwise) {
		updateRectangle(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, textureID);
	}
	else {
		updateRectangleAntiClockwise(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, textureID);
	}
}

/**
 * build the fwMesh
 */
fwMesh* dfMesh::buildMesh(void)
{
	int size = m_vertices.size();

	if (size == 0) {
		return nullptr;
	}

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", &m_vertices[0], 3, size * sizeof(glm::vec3), sizeof(float), false);
	m_geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, &m_uvs[0], 2, size * sizeof(glm::vec2), sizeof(float), false);
	m_geometry->addAttribute("aTextureID", GL_ARRAY_BUFFER, &m_textureID[0], 1, size * sizeof(float), sizeof(float), false);

	m_mesh = new fwMesh(m_geometry, m_material);

	return m_mesh;
}

dfMesh::~dfMesh()
{
	delete m_geometry;
	delete m_mesh;
}
