#include "dfMesh.h"

#include <math.h>
#include <glm/gtx/intersect.hpp>

#include "../framework/fwCollision.h"
#include "../framework/math/fwCylinder.h"

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

void dfMesh::display(fwScene* scene, bool visibility)
{
	m_visible = visibility;

	if (!scene->hasChild(m_mesh)) {
		if (visibility) {
			// add the mesh on the scene
			scene->addChild(m_mesh);
			m_mesh->set_visible(true);
		}
		// no need to add the msh if the supersector is invisible
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
 * if vertices have been touched, rebuild the AABB
 */
void dfMesh::rebuildAABB(void)
{
	m_boundingBox.reset();
	for(auto &vertice: *m_pVertices) {
		m_boundingBox.extend(vertice);
	}
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
 * Return current number of vertices on the mesh
 */
int dfMesh::nbVertices(void)
{
	return m_pVertices->size();
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

	// extend the AABB
	m_boundingBox.extend((*m_pVertices)[p]);
}

/**
 * Update the vertices of a rectangle
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
 * create vertices for a rectangle
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
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
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
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
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
 * create a floor tesselation
 */
void dfMesh::addFloor(std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, float ambient, bool clockwise)
{
	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygons);

	// index the indexes IN the polyines of polygon 
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
	float x, y;
	float xoffset, yoffset;

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
 * Move the elevator floor
 */
void dfMesh::moveFloorTo(float z)
{
	glm::vec3 p = m_mesh->get_position();
	p.y = z / 10.0f;
	position(p);
}

/**
 * Move the elevator floor
 */
void dfMesh::moveCeilingTo(float z)
{
	glm::vec3 p = m_mesh->get_position();
	p.y = z / 10.0f;
	position(p);
}

/**
 * Rotate along Z axis
 */
void dfMesh::rotateZ(float angle)
{
	glm::vec3 rotate(0, angle, 0);
	rotation(rotate);
}

/**
 * force the position in level space
 */
void dfMesh::move(glm::vec3& p)
{
	// convert to gl space
	dfLevel::level2gl(p, m_position);

	position(m_position);
}

/**
 * Override fwMesh::centerVertices to force the bounding box update
 * position is in GL space
 */
void dfMesh::position(glm::vec3& position)
{
	glm::vec3 identity(0.0);

	m_mesh->translate(position);
	updateWorldBoundingBox(nullptr);
}

/**
 * Override fwMesh::centerVertices to force the bounding box update
 */
void dfMesh::rotation(glm::vec3& rotate)
{
	m_mesh->rotate(rotate);
	updateWorldBoundingBox(nullptr);
}

/**
 * Apply the world matrix to the model space bounding box
 */
void dfMesh::updateWorldBoundingBox(dfMesh *parent)
{
	if (m_mesh) {
		if (parent) {
			m_mesh->updateWorldMatrix(parent->m_mesh);
		}
		else {
			m_mesh->updateWorldMatrix(nullptr);
		}
		glm::mat4& worldMatrix = m_mesh->worldMatrix();

		m_worldBoundingBox.apply(m_boundingBox, worldMatrix);
	}
	else {
		glm::mat4& worldMatrix = parent->m_mesh->worldMatrix();

		m_worldBoundingBox.apply(m_boundingBox, worldMatrix);
	}

	for (auto child : m_children) {
		child->updateWorldBoundingBox(this);
	}
}

/**
 * Real-Time Collision Detection
 */
glm::vec3 ClosestPtPointTriangle(glm::vec3& p, glm::vec3& a, glm::vec3& b, glm::vec3 c)
{
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 bc = c - b;

	// Compute parametric position s for projection P’ of P on AB, 
	// P’ = A +s*AB, s = snom/(snom+sdenom) 
	float snom = glm::dot(p - a, ab), sdenom = glm::dot(p - b, a - b);
	// Compute parametric position t for projection P’ of P on AC, 
	// P’ = A +t*AC, s = tnom/(tnom+tdenom) 
	float tnom = glm::dot(p - a, ac), tdenom = glm::dot(p - c, a - c);

	if (snom <= 0.0f && tnom <= 0.0f) return a; // Vertex region early out

	// Compute parametric position u for projection P’ of P on BC, 
	// P’ = B +u*BC, u = unom/(unom+udenom) 
	float unom = glm::dot(p - b, bc), udenom = glm::dot(p - c, b - c);

	if (sdenom <= 0.0f && unom <= 0.0f) return b; // Vertex region early out 
	if (tdenom <= 0.0f && udenom <= 0.0f) return c; // Vertex region early out

	// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0 
	glm::vec3 n = glm::cross(b - a, c - a);
	float vc = glm::dot(n, glm::cross(a - p, b - p));

	// If P outside AB and within feature region of AB, 
	// return projection of P onto AB 
	if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		return a + snom / (snom + sdenom) * ab;

	// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0 
	float va = glm::dot(n, glm::cross(b - p, c - p));
	// If P outside BC and within feature region of BC, 
	// return projection of P onto BC 
	if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		return b + unom / (unom + udenom) * bc;

	// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0 
	float vb = glm::dot(n, glm::cross(c - p, a - p));

	// If P outside CA and within feature region of CA, 
	// return projection of P onto CA 
	if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		return a + tnom / (tnom + tdenom) * ac;

	// P must project inside face region. Compute Q using barycentric coordinates 
	float u = va / (va + vb + vc);
	float v = vb / (va + vb + vc);
	float w = 1.0f - u - v; // = vc / (va + vb + vc) 
	return u * a + v * b + w * c;
}

// Returns true if sphere s intersects triangle ABC, false otherwise. 
// The point p on abc closest to the sphere center is also returned 
int TestSphereTriangle(fwSphere& s, glm::vec3& a, glm::vec3 b, glm::vec3 c, glm::vec3& p)
{ 
	// Find point P on triangle ABC closest to sphere center 
	glm::vec3 center = s.center();
	p = ClosestPtPointTriangle(center, a, b, c);

	// Sphere and triangle intersect if the (squared) distance from sphere 
	// center to point p is less than the (squared) sphere radius 
	glm::vec3 v = p - s.center();
	return glm::dot(v, v) <= s.radius() * s.radius();
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
		// if the plane is passing trough the sphere (the ellipsoid deformed to look like a sphre)
		// get the collision point of the sphere on the plane

		p = center_es + normal * glm::abs(signedDistance);

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

		// if the sum of the 3 new triangles is equal to the gl triangle
		if (abs(tarea1 + tarea2 + tarea3 - tarea) < 0.01) {
			return true;
		}
	}

	return false;
}

/**
 * Test move againsta sphere
 */
bool dfMesh::collide(float step, glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection, std::string& name)
{
	// convert player position (gl world space) into the elevator space (model space)
	glm::vec3 glPosition = glm::vec3(m_mesh->inverseWorldMatrix() * glm::vec4(position, 1.0));
	glm::vec3 glTarget = glm::vec3(m_mesh->inverseWorldMatrix() * glm::vec4(target, 1.0));

	glm::vec3 direction = glm::normalize(glTarget - glPosition);
	glm::vec2 br;
	float distance;
	fwAABBox aabb(glPosition, glTarget);	// convert to AABB for fast test

	if (m_boundingBox.intersect(aabb)) {
		// test if we move through a triangle	(position->segment intersect with triangle)
		for (unsigned int i = 0; i < m_vertices.size(); i += 3) {
			if (glm::intersectRayTriangle(
				glPosition,
				direction,
				m_vertices[i], m_vertices[i + 1], m_vertices[i + 2],
				br,
				distance)) {

				if (distance > 0 && distance < radius) {
					// the intersection point is inside the radius
					intersection = glPosition + distance * direction;

					std::cerr << "dfMesh::collide ray collide with " << name << std::endl;
					return true;
				}
			}
		}
	}

	// convert player position (gl world space) into the elevator space (model space)
	static fwSphere bs;
	bs.set(target, radius);
	fwSphere bsTranformed;
	bsTranformed.applyMatrix4From(m_mesh->inverseWorldMatrix(), &bs);
	aabb = fwAABBox(bsTranformed);	// convert to AABB for fast test
	aabb.m_p.y += step;				// remove the step the player can walk over, below the step it should not trigger a move

	if (m_boundingBox.intersect(aabb)) {
		// now test with the sphere against each triangle
		for (unsigned int i = 0; i < m_vertices.size(); i += 3) {
			if (TestSphereTriangle(bsTranformed, m_vertices[i], m_vertices[i + 1], m_vertices[i + 2], intersection)) {
				// convert back the move point from model space => world space
				bsTranformed.center(intersection);
				bsTranformed.applyMatrix4(m_mesh->worldMatrix());
				intersection = bsTranformed.center();

				glm::vec3 hit = intersection - position;
				direction = target - position;
				if (glm::dot(hit, direction) > 0) {
					// as we checked on a sphere, ensure the intersection is in the direction we want to move, and not on our back
 					std::cerr << "dfMesh::collide sphere collide with " << name << " x=" << intersection.x << " y=" << intersection.y << " z=" << intersection.z << std::endl;
					return true;
				}
			}
		}
	}
	return false;
}

/**
 * Test move against a world AABBox
 * dfMesh::worldBoundingBox is updated everytime the dfMesh moves
 */
bool dfMesh::collide(fwAABBox& box, std::string& name)
{
	if (m_worldBoundingBox.not_init()) {
		m_worldBoundingBox.copy(m_boundingBox);
	}
	return m_worldBoundingBox.intersect(box);
}

/**
 * Test collsuion based on http://www.peroxide.dk/papers/collision/collision.pdf
  */
bool dfMesh::collide(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::string& name, std::list<fwCollisionPoint>& collisions)
{
	fwCylinder cyl(bounding, direction);

	fwAABBox aabb(cyl);	// convert to AABB for fast test

	if (m_worldBoundingBox.intersect(aabb)) {

		// extract the ellipsoid from the cylinder
		glm::vec3 ellipsoid(cyl.height() / 2.0f, cyl.radius(), cyl.radius());
		glm::vec3 center_ws = cyl.position() + direction;
		center_ws.y += cyl.height() / 2.0f;

		// deform the model_space to make the ellipsoid  sphere
		glm::vec3 ellipsoid_space(1.0 / ellipsoid.x, 1.0 / ellipsoid.y, 1.0 / ellipsoid.z);

		// convert player position (gl world space) into the elevator space (model space)
		glm::vec3 center_ms = glm::vec3(m_mesh->inverseWorldMatrix() * glm::vec4(center_ws, 1.0));
		// and convert to ellipsod space
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
				if ( delta > 0.9) {
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::BOTTOM, intersection));
					c=fwCollisionLocation::BOTTOM;
				}
				else if (delta < -0.9) {
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::TOP, intersection));
					c = fwCollisionLocation::TOP;
				}
				else if (delta  > 0.5 && d > 0) {
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::FRONT_BOTTOM, intersection));
					c = fwCollisionLocation::FRONT_BOTTOM;
				}
				else if (delta < -0.5 && d > 0) {
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::FRONT_TOP, intersection));
					c = fwCollisionLocation::FRONT_TOP;
				}
				else if (d <= 0) {
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::BACK, intersection));
					c = fwCollisionLocation::BACK;
				}
				else if (d <= 0.4) {
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::LEFT, intersection));
					c = fwCollisionLocation::LEFT;
				}
				else {
					float l = glm::length(intersection - center_ws);
					float l1 = glm::length(origin - center_es);
					collisions.push_back(fwCollisionPoint(fwCollisionLocation::FRONT, intersection));
					c = fwCollisionLocation::FRONT;
				}
				std::cerr << "dfMesh::collide ellipoid collides with " << name << " on " << (int)c <<std::endl;

				return true;
			}
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
 * Registera sub fwmesh
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
 * Move all vertices as offset of the center
 */
void dfMesh::moveVertices(glm::vec3& center)
{
	// convert to gl space
	dfLevel::level2gl(center, m_position);

	for (auto &vertice : m_vertices) {
		vertice -= m_position;
	}

	m_mesh->translate(m_position);
	m_mesh->updateVertices();	// reupload vertices to GPU
	rebuildAABB();
}

/**
 * Move all vertices to orbit around the center XZ of the geometry
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
	m_mesh->updateVertices();	// reupload vertices to GPU

	rebuildAABB();

	// convert to gl space
	dfLevel::gl2level(center, target);
}

/**
 * Move all vertices to orbit around the center XZ of the geometry
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
	m_mesh->updateVertices();	// reupload vertices to GPU

	rebuildAABB();

	// convert to gl space
	dfLevel::gl2level(center, target);
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
	m_mesh->set_name(m_name);
	position(m_position);

	return m_mesh;
}

/**
 * Start playing a sound or check if it plays
 */
bool dfMesh::play(dfVOC* voc)
{
	if (m_mesh) {
		if (voc) {
			std::cerr << "dfMesh::play " << voc->name() << std::endl;
		}
		return m_mesh->play(voc->sound());
	}

	return false;
}

/**
 * Stop playing a sound (or all sound if nullptr)
 */
void dfMesh::stop(dfVOC* voc)
{
	if (m_mesh) {
		if (voc) {
			std::cerr << "dfMesh::stop " << voc->name() << std::endl;
		}
		m_mesh->stop(voc->sound());
	}
}

dfMesh::~dfMesh()
{
	delete m_geometry;
	delete m_mesh;
}
