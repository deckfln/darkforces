#include "dfMesh.h"

#include <math.h>
#include <glm/gtx/intersect.hpp>

#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaCollisionPoint.h"

#include "dfSector.h"
#include "dfBitmap.h"
#include "dfLevel.h"
#include "dfVOC.h"
#include "dfLevel.h"

#include "../include/earcut.hpp"

dfMesh::dfMesh(fwMaterial* material, std::vector<dfBitmap*>& bitmaps):
	fwMesh(material),
	m_pVertices(&m_vertices),
	m_pUvs(&m_uvs),
	m_pTextureIDs(&m_textureID),
	m_pAmbientLights(&m_ambient),
	m_bitmaps(bitmaps)
{
}

dfMesh::dfMesh(dfSuperSector* ssector, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uvs, std::vector<float>* textureIDs, std::vector <float>* ambientLights, std::vector<dfBitmap*>& textures):
	fwMesh(),
	m_supersector(ssector),
	m_pVertices(vertices),
	m_pUvs(uvs),
	m_pTextureIDs(textureIDs),
	m_pAmbientLights(ambientLights),
	m_bitmaps(textures)
{
}

/**
 * Create a dfMesh sharing the same attributes as the parent
 */
dfMesh::dfMesh(dfMesh* source):
	fwMesh(),
	m_bitmaps(source->m_bitmaps),
	m_supersector(source->m_supersector),
	m_pVertices(&source->m_vertices),
	m_pAmbientLights(&source->m_ambient),
	m_pTextureIDs(&source->m_textureID),
	m_pUvs(&source->m_uvs),
	m_source(source)
{
	// TODO: the object cannot be added as a child of a fwMesh as it is NOT an indepent fwMesh
	// it is actually added at the end of the Parent mesh
	// source->addChild(this);
}

/**
 * Build the AABB for the mesh
 */
const GameEngine::AABBoxTree& dfMesh::modelAABB(void)
{
	if (m_modelAABB.not_init()) {
		for (auto& vertice : m_vertices) {
			m_modelAABB.extend(vertice);
		}

		m_modelAABB.geometry(&m_vertices, 0, m_vertices.size());
	}

	return m_modelAABB;
}

/**
 * Create a hierarchy of AABB pointing to triangles
 */
void dfMesh::addModelAABB(GameEngine::AABBoxTree* child)
{
	m_modelAABB.add(child);
}

void dfMesh::display(fwScene* scene, bool visibility)
{
	if (!scene->hasChild(this)) {
			// add the mesh on the scene
			scene->addChild(this);
		// no need to add the mesh if the supersector is invisible
	}
	visible(visibility);
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
	int p = m_pVertices->size();
	m_pVertices->resize(p + i);
	m_pUvs->resize(p + i);
	m_pTextureIDs->resize(p + i);
	m_pAmbientLights->resize(p + i);
	return p;
}

/**
 *
 */
void dfMesh::resizeGeometry(void)
{
	// if this is an original geometry, test the local geometry
	// if this is a shadow mesh, use the source geometry
	fwGeometry* mygeometry = m_geometry;
	if (m_source != nullptr) {
		mygeometry = m_source->m_geometry;
	}

	if (mygeometry != nullptr) {
		// oh dear, we are resizing an existing geometry, so resize each attribute
		mygeometry->resizeAttribute("aPos", &m_pVertices->at(0), m_pVertices->size());
		mygeometry->resizeAttribute("aTexCoord", &m_pUvs->at(0), m_pUvs->size());
		mygeometry->resizeAttribute("aTextureID", &m_pTextureIDs->at(0), m_pTextureIDs->size());
		if (m_pAmbientLights->size() > 0) {
			mygeometry->resizeAttribute("aAmbient", &m_pAmbientLights->at(0), m_pAmbientLights->size());
		}
	}

}

/**
 * Return current number of vertices's on the mesh
 */
int dfMesh::nbVertices(void)
{
	return m_pVertices->size();
}

/**
 * address of the vertex
 */
std::vector<glm::vec3> *dfMesh::vertice(void)
{
	return m_pVertices;
}

/**
 * Setup a vertice
 */
void dfMesh::setVertice(int p, float x, float y, float z, float xoffset, float yoffset, int textureID, float ambient)
{
	(*m_pVertices)[p].x = x / dfOpengl2space;
	(*m_pVertices)[p].z = y / dfOpengl2space;
	(*m_pVertices)[p].y = z / dfOpengl2space;
	(*m_pUvs)[p] = glm::vec2(xoffset, yoffset);
	(*m_pTextureIDs)[p] = (float)textureID;
	(*m_pAmbientLights)[p] = ambient / 32.0f;
}

/**
 * Update the vertices's of a rectangle
 */
void dfMesh::updateRectangle(int p, 
	glm::vec3& pStart,
	glm::vec3& pEnd,
	glm::vec2& offset,
	glm::vec2& size,
	int textureID,
	float ambient)
{
	glm::vec2 texEnd = offset + size;

	// first triangle
	setVertice(p, pStart.x, pStart.y, pStart.z, offset.x, offset.y, textureID, ambient);
	setVertice(p + 1, pEnd.x, pEnd.y, pStart.z, texEnd.x, offset.y, textureID, ambient);
	setVertice(p + 2, pEnd.x, pEnd.y, pEnd.z, texEnd.x, texEnd.y, textureID, ambient);

	// second triangle
	setVertice(p + 3, pStart.x, pStart.y, pStart.z, offset.x, offset.y, textureID, ambient);
	setVertice(p + 4, pEnd.x, pEnd.y, pEnd.z, texEnd.x, texEnd.y, textureID, ambient);
	setVertice(p + 5, pStart.x, pStart.y, pEnd.z, offset.x, texEnd.y, textureID, ambient);
}

/**
 * update the texture attribute at position i
 */
void dfMesh::updateTexture(uint32_t i, float id)
{
	(*m_pTextureIDs)[i] = id;
}

/**
 * Update the vertices of a rectangle
 */
void dfMesh::updateRectangleAntiClockwise(int p, 
	glm::vec3& pStart, 
	glm::vec3& pEnd, 
	glm::vec2& offset,
	glm::vec2& size, 
	int textureID, 
	float ambient)
{
	glm::vec2 texEnd = offset + size;

	// first triangle
	setVertice(p, pStart.x, pStart.y, pStart.z, offset.x, offset.y, textureID, ambient);
	setVertice(p + 2, pEnd.x, pEnd.y, pStart.z, texEnd.x, offset.y, textureID, ambient);
	setVertice(p + 1, pEnd.x, pEnd.y, pEnd.z, texEnd.x, texEnd.y, textureID, ambient);

	// second triangle
	setVertice(p + 3, pStart.x, pStart.y, pStart.z, offset.x, offset.y, textureID, ambient);
	setVertice(p + 5, pEnd.x, pEnd.y, pEnd.z, texEnd.x, texEnd.y, textureID, ambient);
	setVertice(p + 4, pStart.x, pStart.y, pEnd.z, offset.x, texEnd.y, textureID, ambient);
}

/***
 * create vertices's for a rectangle
 */
int dfMesh::addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, float ambient)
{
	int p = start;

	if (start == -1) {
		// add a new rectangle
		p = resize(6);
	}

	glm::vec3 pstart(
		sector->m_vertices[wall->m_left].x, 
		sector->m_vertices[wall->m_left].y, z);

	glm::vec3 pend(
		sector->m_vertices[wall->m_right].x, 
		sector->m_vertices[wall->m_right].y, z1);

	// deal with the wall texture
	float bitmapID = wall->m_tex[texture].x;

	dfBitmapImage* image = m_bitmaps[(int)bitmapID]->getImage();

	float length = sqrt(pow(pstart.x - pend.x, 2) + pow(pstart.y - pend.y, 2));
	float xpixel = (float)image->m_width;
	float ypixel = (float)image->m_height;

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	glm::vec2 size(
		length * 8.0f / xpixel,
		abs(z1 - z) * 8.0f / ypixel);

	// get local texture offset on the wall
	// TODO: current supposition : offset x 1 => 1 pixel from the beginning on XXX width pixel texture
	glm::vec2 offset(
		(wall->m_tex[texture].y * 8.0f) / xpixel,
		(wall->m_tex[texture].z * 8.0f) / ypixel);

	// Handle request to flip the texture
	bool flipTexture = wall->flag1(dfWallFlag::FLIP_TEXTURE_HORIZONTALLY);
	if (flipTexture) {
		offset.x = 1.0f - offset.x;
		size.x = -size.x;
	}

	updateRectangle(p, pstart, pend, offset, size, image->m_textureID, ambient);

	if (start >= 0) {
		return 6;	// we updated the rectangle, move to the next rectangle
	}

	return 0;	// we added a new rectangle, keep the index at -1
}


/**
 * create a simple opengl Rectangle
 */
void dfMesh::addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, float ambient, bool clockwise)
{
	// add a new rectangle
	int p = resize(6);

	glm::vec3 pstart(
		sector->m_vertices[wall->m_left].x, 
		sector->m_vertices[wall->m_left].y, z);

	glm::vec3 pend(
		sector->m_vertices[wall->m_right].x, 
		sector->m_vertices[wall->m_right].y, z1);

	// deal with the wall texture
	float bitmapID = texture.x;

	dfBitmapImage* image = m_bitmaps[(int)bitmapID]->getImage();
	float xpixel = 0;
	float ypixel = 0;

	if (image != nullptr) {
		xpixel = (float)image->m_width;
		ypixel = (float)image->m_height;
	}

	float length = sqrt(pow(pstart.x - pend.x, 2) + pow(pstart.y - pend.y, 2));

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	glm::vec2 size(
		length * 8.0f / xpixel, 
		abs(z1 - z) * 8.0f / ypixel);

	// get local texture offset on the wall
	// TODO: current supposition : offset x 1 => 1 pixel from the beginning on XXX width pixel texture
	glm::vec2 offset(
		(texture.y * 8.0f) / xpixel, 
		(texture.z * 8.0f) / ypixel);

	// Handle request to flip the texture
	bool flipTexture = wall->flag1(dfWallFlag::FLIP_TEXTURE_HORIZONTALLY);
	if (flipTexture) {
		offset.x = 1.0f - offset.x;
		size.x = -size.x;
	}

	if (clockwise) {
		updateRectangle(p, pstart, pend, offset, size, image->m_textureID, ambient);
	}
	else {
		updateRectangleAntiClockwise(p, pstart, pend, offset, size, image->m_textureID, ambient);
	}
}

/**
 * create a floor tessellation
 */
void dfMesh::addFloor(std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, float ambient, bool clockwise)
{
	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygons);

	// index the indexes IN the poly-lines of polygon 
	std::vector<Point> vertices;

	for (auto& poly : polygons) {
		for (auto& p : poly) {
			vertices.push_back(p);
		}
	}

	// resize the opengl buffers
	int cvertices = indices.size();	
	int p = resize(cvertices);

	// use axis aligned texture UV, on a 8x8 grid
	// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
	dfBitmapImage* image = m_bitmaps[(int)texture.r]->getImage();
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
		// TODO: current supposition : offset x 1 => 1 pixel from the beginning on XXX width pixel texture
		float xoffset = ((vertices[index][0] + texture.g) * 8.0f) / xpixel;
		float yoffset = ((vertices[index][1] + texture.b) * 8.0f) / ypixel;


		if (!clockwise) {
			// reverse vertices's 2 and 3 to look upward
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
	float x, y;
	float xoffset, yoffset;

	for (unsigned int i = 0; i < 6; i++) {
		// get local texture offset on the floor
		// TODO: current supposition : offset x 1 => 1 pixel from the beginning on XXX width pixel texture
		switch (i) {
		case 0:	t = -1; t1 = -1; break;
		case 1:	t = 1; t1 = -1;  break;
		case 2:	t = -1; t1 = 1; break;
		case 3:	t = -1; t1 = 1;  break;
		case 4:	t = 1; t1 = -1;  break;
		case 5:	t = 1; t1 = 1;  break;
		}

		x = t * width;
		y = t1 * width;

		xoffset = (x * 0.5f) / xpixel;
		yoffset = (y * 0.5f) / ypixel;

		setVertice(i,
			x,
			y,
			0,
			xoffset, yoffset,
			image->m_textureID,
			32.0f);
	}
}

/**
 * Update the Ambient buffer attribute
 */
void dfMesh::changeAmbient(float ambient, int start, int len)
{
	for (int i = start; i < start + len; i++) {
		m_ambient[i] = ambient;
	}
	m_geometry->updateAttribute("aAmbient", 0);
}

/**
 * Update the TextureIDs Attribute on the GPU
 */
void dfMesh::updateGeometryTextures(int start, int nb)
{
	m_geometry->updateAttribute("aTextureID", start, nb);
}

/**
 * Move all vertices's as offset of the center
 */
void dfMesh::moveVertices(glm::vec3& center)
{
	// convert to opengl space
	glm::vec3 pos;
	dfLevel::level2gl(center, pos);

	for (auto &vertice : m_vertices) {
		vertice -= pos;
	}

	translate(pos);
	updateVertices();	// re-upload vertices's to GPU
}

/**
 * Move all vertices's to orbit around the center XZ of the geometry
 */
void dfMesh::centerOnGeometryXZ(glm::vec3& target)
{
	glm::vec3 center(0,0,0);
	for (auto& vertice : m_vertices) {
		center += vertice;
	}
	center /= m_vertices.size();

	center.y = 0;

	for (auto& vertice : m_vertices) {
		vertice -= center;
	}

	glm::vec3 pos = position();
	pos.x += center.x;	//take count of the existing position (for SPIN1)
	pos.z += center.z;

	translate(pos);
	updateVertices();	// re-upload vertices's to GPU

	// convert to opengl space
	dfLevel::gl2level(center, target);
}

/**
 * Move all vertices's to orbit around the center XZ of the geometry
 */
void dfMesh::centerOnGeometryXYZ(glm::vec3& target)
{
	glm::vec3 center(0, 0, 0);
	for (auto& vertice : m_vertices) {
		center += vertice;
	}
	center /= m_vertices.size();

	for (auto& vertice : m_vertices) {
		vertice -= center;
	}

	glm::vec3 pos = position();
	pos.x += center.x;	//take count of the existing position (for SPIN1)
	pos.z += center.z;

	translate(pos);
	updateVertices();	// re-upload vertices's to GPU

	// convert to opengl space
	dfLevel::gl2level(center, target);
}

/**
 * override the default position function : return the real position for a copy mesh
 */
const glm::vec3& dfMesh::position(void)
{
	if (m_source) {
		return m_source->fwObject3D::position();
	}

	return fwObject3D::position();
}

/**
 * build the fwMesh
 */
dfMesh* dfMesh::buildMesh(void)
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

	// record the full size of the geometry
	m_modelAABB.geometry(&m_vertices, 0, size);
	return this;
}

dfMesh::~dfMesh()
{
	if (m_geometry)
		delete m_geometry;

	if (m_CompMesh)
		delete m_CompMesh;
}
