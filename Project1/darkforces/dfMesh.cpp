#include "dfMesh.h"

#include <math.h>
#include <glm/gtx/intersect.hpp>

#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaCollisionPoint.h"

#include "dfSector.h"
#include "dfBitmap.h"
#include "dfLevel.h"
#include "dfVOC.h"

#include "../include/earcut.hpp"

dfMesh::dfMesh(fwMaterial* material, std::vector<dfBitmap*>& bitmaps):
	m_material(material),
	m_pVertices(&m_vertices),
	m_pUvs(&m_uvs),
	m_pTextureIDs(&m_textureID),
	m_pAmbientLights(&m_ambient),
	m_bitmaps(bitmaps)
{
}

dfMesh::dfMesh(dfSuperSector* ssector, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uvs, std::vector<float>* textureIDs, std::vector <float>* ambientLights, std::vector<dfBitmap*>& textures):
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
dfMesh::dfMesh(dfMesh* parent):
	m_bitmaps(parent->m_bitmaps),
	m_supersector(parent->m_supersector),
	m_pVertices(&parent->m_vertices),
	m_pAmbientLights(&parent->m_ambient),
	m_pTextureIDs(&parent->m_textureID),
	m_pUvs(&parent->m_uvs),
	m_parent(parent)
{
	parent->addChild(this);
}

/**
 * Build the AABB for the mesh
 */
const GameEngine::AABBoxTree& dfMesh::modelAABB(void)
{
	for (auto& vertice : m_vertices) {
		m_modelAABB.extend(vertice);
	}

	m_modelAABB.geometry(&m_vertices[0], m_vertices.size());

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
	m_visible = visibility;

	if (!scene->hasChild(m_mesh)) {
		if (visibility) {
			// add the mesh on the scene
			scene->addChild(m_mesh);
			m_mesh->set_visible(true);
		}
		// no need to add the mesh if the supersector is invisible
	}
	else {
		m_mesh->set_visible(visibility);
	}

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
 * Return current number of vertices's on the mesh
 */
int dfMesh::nbVertices(void)
{
	return m_pVertices->size();
}

/**
 * address of the vertex
 */
glm::vec3 const* dfMesh::vertice(uint32_t index)
{
	if (index > m_pVertices->size()) {
		return nullptr;
	}

	return &m_pVertices->at(index);
}

/**
 * set the name of the dfMesh and the associated fwMesh
 */
void dfMesh::name(std::string& name)
{
	m_name = name;
	if (m_mesh) {
		m_mesh->set_name(name);
	}
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

	for (auto poly : polygons) {
		for (auto p : poly) {
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
 * // extended segment collision test after a successful AABB collision
 */
bool dfMesh::collisionSegmentTriangle(const glm::vec3& p, const glm::vec3& q, std::list<gaCollisionPoint>& collisions)
{
	// test all triangles
	return collisionSegmentTriangle(p, q, collisions, 0, m_vertices.size());
}

/**
 * extended segment collision on limited set of triangles
 */
bool dfMesh::collisionSegmentTriangle(const glm::vec3& p, const glm::vec3& q, std::list<gaCollisionPoint>& collisions, int firstVertex, int nbVertices)
{
	float u, v, w, t;
	glm::vec3 collision;

	for (unsigned int i = firstVertex; i < nbVertices; i += 3) {
		if (IntersectSegmentTriangle(p, q,
			m_vertices[i], m_vertices[i + 1], m_vertices[i + 2],
			u, v, w, t
		)) {
			// rebuild collision point
			collision = u * m_vertices[i] + v * m_vertices[i + 1] + w * m_vertices[i + 2];
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::BACK, collision, nullptr));
		};
	}

	return false;
}

/**
 * Sphere/Triangle intersection (other version)
 */
static bool intersectSphereTriangle(const glm::vec3& center_es, const glm::vec3& a, const glm::vec3 b, const glm::vec3 c, glm::vec3& p)
{
	// plane equation
	glm::vec3 normal;
	glm::vec4 plane;
	float signedDistance;

	// triangle equation
	glm::vec3 tr0;
	glm::vec3 tr1;
	float tarea, tarea1, tarea2, tarea3;

	// first, test the intersection with the triangle plane
	normal = glm::normalize(glm::cross(b - a, c - a));
	plane = glm::vec4(normal.x, normal.y, normal.z, -(normal.x * a.x + normal.y * a.y + normal.z * a.z));
	signedDistance = glm::dot(center_es, normal) + plane.w;

	if (signedDistance >= -1.0f && signedDistance <= 1.0f) {
		// if the plane is passing trough the sphere (the ellipsoid deformed to look like a sphere)
		// get the collision point of the sphere on the plane

		p = center_es - normal * glm::abs(signedDistance);

		// test if the collision origin in INSIDE the triangle

		// area of the GL triangle
		tr0 = a - b;
		tr1 = c - b;
		tarea = glm::length(glm::cross(tr0, tr1));

		// area of the triangles using the intersection point as vertex
		tr0 = a - p;
		tr1 = b - p;
		tarea1 = glm::length(glm::cross(tr0, tr1));

		tr0 = b - p;
		tr1 = c - p;
		tarea2 = glm::length(glm::cross(tr0, tr1));

		tr0 = c - p;
		tr1 = a - p;
		tarea3 = glm::length(glm::cross(tr0, tr1));

		// if the sum of the 3 new triangles is equal to the opengl triangle
		if (abs(tarea1 + tarea2 + tarea3 - tarea) < 0.01) {
			return true;
		}
	}

	return false;
}

/**
 * Test collision based on http://www.peroxide.dk/papers/collision/collision.pdf
 */
bool dfMesh::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::string& name, std::list<gaCollisionPoint>& collisions)
{
	fwCylinder cyl(bounding, direction);

	fwAABBox aabb(cyl);	// convert to AABB for fast test

	// extract the ellipsoid from the cylinder
	glm::vec3 ellipsoid(cyl.height() / 2.0f, cyl.radius(), cyl.radius());
	glm::vec3 center_ws = cyl.position();
	center_ws.y += cyl.height() / 2.0f;

	// deform the model_space to make the ellipsoid  sphere
	glm::vec3 ellipsoid_space(1.0 / ellipsoid.x, 1.0 / ellipsoid.y, 1.0 / ellipsoid.z);

	// convert player position (opengl world space) into the elevator space (model space)
	glm::vec3 center_ms = glm::vec3(m_mesh->inverseWorldMatrix() * glm::vec4(center_ws, 1.0));
	// and convert to ellipsoid space
	glm::vec3 center_es = center_ms * ellipsoid_space;

	// test each triangle vs the ellipsoid
	glm::vec3 v1_es, v2_es, v3_es;
	glm::vec3 origin;

	for (unsigned int i = 0; i < m_vertices.size(); i += 3) {

		// convert each vertex to the ellipsoid space
		v1_es = m_vertices[i] * ellipsoid_space;
		v2_es = m_vertices[i + 1] * ellipsoid_space;
		v3_es = m_vertices[i + 2] * ellipsoid_space;

		if (intersectSphereTriangle(center_es, v1_es, v2_es, v3_es, origin)) {
			// the intersection point is inside the triangle

			// convert the intersection point back to model space
			intersection = origin / ellipsoid_space;

			// convert from (model space) intersection to (level space) intersection
			intersection = glm::vec3(m_mesh->worldMatrix() * glm::vec4(intersection, 1.0));

			// position of the intersection compared to the direction
			glm::vec3 AC = glm::normalize(intersection - center_ws);
			float d = glm::dot(glm::normalize(direction), AC);

			float delta = center_es.y - origin.y;
			fwCollisionLocation c;
			if (delta > 0.9) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, intersection, nullptr));
				c = fwCollisionLocation::BOTTOM;
			}
			else if (delta < -0.9) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::TOP, intersection, nullptr));
				c = fwCollisionLocation::TOP;
			}
			else if (delta > 0.5 && d > 0) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT_BOTTOM, intersection, nullptr));
				c = fwCollisionLocation::FRONT_BOTTOM;
			}
			else if (delta < -0.5 && d > 0) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT_TOP, intersection, nullptr));
				c = fwCollisionLocation::FRONT_TOP;
			}
			else if (d <= 0) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::BACK, intersection, nullptr));
				c = fwCollisionLocation::BACK;
			}
			else if (d <= 0.4) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::LEFT, intersection, nullptr));
				c = fwCollisionLocation::LEFT;
			}
			else {
				float l = glm::length(intersection - center_ws);
				float l1 = glm::length(origin - center_es);
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, intersection, nullptr));
				c = fwCollisionLocation::FRONT;
			}
			std::cerr << "dfMesh::checkCollision ellipsoid collides with " << name << " on " << (int)c << std::endl;

			return true;
		}
	}

	return false;
}

/**
 * Test if the mesh and the parent mesh are visible
 */
bool dfMesh::visible(void)
{
	if (m_parent)
		return m_visible && m_parent->m_visible;

	return m_visible;
}

void dfMesh::visible(bool status)
{
	m_visible = status;
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
 * Add a child dfmesh sharing the same attributes
 */
void dfMesh::addChild(dfMesh* mesh)
{
	m_children.push_back(mesh);
}

/**
 * Register a sub fwmesh
 */
void dfMesh::addMesh(fwMesh* mesh)
{
	m_mesh->addChild(mesh);
}

/**
 * Force the Z order of the mesh
 */
void dfMesh::zOrder(int z)
{
	m_mesh->zOrder(z);
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
	dfLevel::level2gl(center, m_position);

	for (auto &vertice : m_vertices) {
		vertice -= m_position;
	}

	m_mesh->translate(m_position);
	m_mesh->updateVertices();	// re-upload vertices's to GPU
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

	m_position.x += center.x;	//take count of the existing position (for SPIN1)
	m_position.z += center.z;

	m_mesh->translate(m_position);
	m_mesh->updateVertices();	// re-upload vertices's to GPU

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

	m_position.x += center.x;	//take count of the existing position (for SPIN1)
	m_position.z += center.z;

	m_mesh->translate(m_position);
	m_mesh->updateVertices();	// re-upload vertices's to GPU

	// convert to opengl space
	dfLevel::gl2level(center, target);
}

/**
 * build the fwMesh
 */
GameEngine::ComponentMesh* dfMesh::buildMesh(void)
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

	m_mesh = new GameEngine::ComponentMesh(m_geometry, m_material);
	m_mesh->set_name(m_name);

	return m_mesh;
}

dfMesh::~dfMesh()
{
	delete m_geometry;
	delete m_mesh;
}
