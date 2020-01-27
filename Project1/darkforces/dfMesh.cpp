#include "dfMesh.h"

#include "dfSector.h"
#include "dfBitmap.h"

#include "../include/earcut.hpp"

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
 * Resize the buffers
 */
int dfMesh::resize(int i)
{
	int p = m_vertices.size();
	m_vertices.resize(p + i);
	m_uvs.resize(p + i);
	m_textureID.resize(p + i);
	m_ambient.resize(p + i);

	return p;
}

/**
 * Setup a vertice
 */
void dfMesh::setVertice(int p, float x, float y, float z, float xoffset, float yoffset, int textureID, float ambient)
{
	m_vertices[p].x = x / dfOpengl2space;
	m_vertices[p].z = y / dfOpengl2space;
	m_vertices[p].y = z / dfOpengl2space;
	m_uvs[p] = glm::vec2(xoffset, yoffset);
	m_textureID[p] = (float)textureID;
	m_ambient[p] = ambient;
}

/**
 * Update the vertices of a rectangle
 */
void dfMesh::updateRectangle(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, int textureID, float ambient)
{
	// first triangle
	setVertice(p, x, y, z, xoffset, yoffset, textureID, ambient);
	setVertice(p + 1, x1, y1, z, width + xoffset, yoffset, textureID, ambient);
	setVertice(p + 2, x1, y1, z1, width + xoffset, height + yoffset, textureID, ambient);

	// second triangle
	setVertice(p + 3, x, y, z, xoffset, yoffset, textureID, ambient);
	setVertice(p + 4, x1, y1, z1, width + xoffset, height + yoffset, textureID, ambient);
	setVertice(p + 5, x, y, z1, xoffset, height + yoffset, textureID, ambient);
}

/**
 * Update the vertices of a rectangle
 */
void dfMesh::updateRectangleAntiClockwise(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, int textureID, float ambient)
{
	// first triangle
	setVertice(p, x, y, z, xoffset, yoffset, textureID, ambient);
	setVertice(p + 2, x1, y1, z, width + xoffset, yoffset, textureID, ambient);
	setVertice(p + 1, x1, y1, z1, width + xoffset, height + yoffset, textureID, ambient);

	// second triangle
	setVertice(p + 3, x, y, z, xoffset, yoffset, textureID, ambient);
	setVertice(p + 5, x1, y1, z1, width + xoffset, height + yoffset, textureID, ambient);
	setVertice(p + 4, x, y, z1, xoffset, height + yoffset, textureID, ambient);
}

/***
 * create vertices for a rectangle
 */
int dfMesh::addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfBitmap*>& bitmaps, float ambient)
{
	int p = start;

	if (start == -1) {
		// add a new rectangle
		p = resize(6);
	}

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	// deal with the wall texture
	float bitmapID = wall->m_tex[texture].x;

	dfBitmapImage* image = bitmaps[(int)bitmapID]->getImage();

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = (float)image->m_width;
	float ypixel = (float)image->m_height;

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	float height = abs(z1 - z) * 8.0f / ypixel;
	float width = length * 8.0f / xpixel;

	// get local texture offset on the wall
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
	float xoffset = (wall->m_tex[texture].y * 8.0f) / xpixel;
	float yoffset = (wall->m_tex[texture].z * 8.0f) / ypixel;

	updateRectangle(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, image->m_textureID, ambient);

	if (start >= 0) {
		return 6;	// we updated the rectangle, move to the next rectangle
	}

	return 0;	// we added a new rectangle, keep the index at -1
}


/**
 * create a simple opengl Rectangle
 */
void dfMesh::addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, std::vector<dfBitmap*>& bitmaps, float ambient, bool clockwise)
{
	// add a new rectangle
	int p = resize(6);

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	// deal with the wall texture
	float bitmapID = texture.x;

	dfBitmapImage* image = bitmaps[(int)bitmapID]->getImage();
	float xpixel = 0;
	float ypixel = 0;

	if (image != nullptr) {
		xpixel = (float)image->m_width;
		ypixel = (float)image->m_height;
	}

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	float height = abs(z1 - z) * 8.0f / ypixel;
	float width = length * 8.0f / xpixel;

	// get local texture offset on the wall
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
	float xoffset = (texture.y * 8.0f) / xpixel;
	float yoffset = (texture.z * 8.0f) / ypixel;

	if (clockwise) {
		updateRectangle(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, image->m_textureID, ambient);
	}
	else {
		updateRectangleAntiClockwise(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, image->m_textureID, ambient);
	}
}

/**
 * create a floor tesselation
 */
void dfMesh::addFloor(std::vector<Point>& vertices, std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, std::vector<dfBitmap*>& bitmaps, float ambient, bool clockwise)
{
	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygons);

	// resize the opengl buffers
	int cvertices = indices.size();	
	int p = resize(cvertices);

	// use axis aligned texture UV, on a 8x8 grid
	// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
	dfBitmapImage* image = bitmaps[(int)texture.r]->getImage();
	float xpixel = 0;
	float ypixel = 0;
	if (image != nullptr) {
		xpixel = (float)image->m_width;
		ypixel = (float)image->m_height;
	}
	// warning, triangles are looking downward
	int currentVertice = 0, j;
	for (unsigned int i = 0; i < indices.size(); i++) {
		int index = indices[i];

		// get local texture offset on the floor
		// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
		float xoffset = ((vertices[index][0] + texture.g) * 8.0f) / xpixel;
		float yoffset = ((vertices[index][1] + texture.b) * 8.0f) / ypixel;


		if (!clockwise) {
			// reverse vertices 2 and 3 to look upward
			switch (currentVertice) {
			case 1: j = 1; break;
			case 2: j = -1; break;
			default: j = 0; break;
			}

			setVertice(p + j, 
				vertices[index][0], 
				vertices[index][1],
				z, 
				xoffset, yoffset, 
				image->m_textureID, 
				ambient);
		}
		else {
			setVertice(p,
				vertices[index][0],
				vertices[index][1],
				z,
				xoffset, yoffset,
				image->m_textureID,
				ambient);
		}

		p++;
		currentVertice = (currentVertice + 1) % 3;
	}
}

/**
 * create a ceiling plane
 */
void dfMesh::addPlane(float width, dfBitmapImage* image)
{
	int p = resize(6);

	// use axis aligned texture UV, on a 8x8 grid
	// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
	float xpixel = 0;
	float ypixel = 0;
	if (image != nullptr) {
		xpixel = (float)image->m_width;
		ypixel = (float)image->m_height;
	}
	// warning, triangles are looking downward
	int t = 0, t1;

	for (unsigned int i = 0; i < 6; i++) {
		// get local texture offset on the floor
		// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
		switch (i) {
		case 0:	t = -1; t1 = -1; break;
		case 1:	t = 1; t1 = -1;  break;
		case 2:	t = -1; t1 = 1; break;
		case 3:	t = -1; t1 = 1;  break;
		case 4:	t = 1; t1 = -1;  break;
		case 5:	t = 1; t1 = 1;  break;
		}

		float xoffset = (m_vertices[i].x * 8.0f) / xpixel;
		float yoffset = (m_vertices[i].z * 8.0f) / ypixel;

		setVertice(i,
			t * width,
			t1 * width,
			0,
			xoffset, yoffset,
			image->m_textureID,
			1.0f);
	}
}

/**
 * Move the elevator floor
 */
void dfMesh::moveFloorTo(float z)
{
	glm::vec3 p = m_mesh->get_position();
	p.y = z / 10.0f;
	m_mesh->position(p);
}

/**
 * Move the elevator floor
 */
void dfMesh::moveCeilingTo(float z)
{
	glm::vec3 p = m_mesh->get_position();
	p.y = z / 10.0f;
	m_mesh->position(p);
}

/**
 * Rotate along Z axis
 */
void dfMesh::rotateZ(float angle)
{
	glm::vec3 rotate(0, angle, 0);
	m_mesh->rotate(rotate);
}

/**
 *
 */
void dfMesh::move(glm::vec3 position)
{
	m_position = glm::vec3(position.x, position.z, position.y) / 10.0f;

	m_mesh->position(m_position);
}

/**
 * Move all vertices as offset of the center
 */
void dfMesh::moveVertices(glm::vec3& center)
{
	m_position = glm::vec3(center.x, center.z, center.y) / 10.0f;

	for (auto &vertice : m_vertices) {
		vertice -= m_position;
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
	if (m_ambient.size() > 0) {
		m_geometry->addAttribute("aAmbient", GL_ARRAY_BUFFER, &m_ambient[0], 1, size * sizeof(float), sizeof(float), false);
	}

	m_mesh = new fwMesh(m_geometry, m_material);
	m_mesh->position(m_position);

	return m_mesh;
}

dfMesh::~dfMesh()
{
	delete m_geometry;
	delete m_mesh;
}
