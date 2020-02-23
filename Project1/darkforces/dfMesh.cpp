#include "dfMesh.h"

#include <glm/gtx/intersect.hpp>
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
 * if vertices have been touched, rebuild the AABB
 */
void dfMesh::rebuildAABB(void)
{
	m_boundingBox.reset();
	for(auto &vertice: m_vertices) {
		m_boundingBox.extend(vertice);
	}
}

/**
 * find the center of the mesh, base point for translations
 */
void dfMesh::findCenter(void)
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

	// extend the AABB
	m_boundingBox.extend(m_vertices[p]);
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
	m_mesh->translate(p);
}

/**
 * Move the elevator floor
 */
void dfMesh::moveCeilingTo(float z)
{
	glm::vec3 p = m_mesh->get_position();
	p.y = z / 10.0f;
	m_mesh->translate(p);
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
 * force the position
 */
void dfMesh::move(glm::vec3& position)
{
	m_position = glm::vec3(position.x, position.z, position.y) / 10.0f;

	m_mesh->translate(m_position);
}

/**
 * translate the object
 */
void dfMesh::translate(glm::vec3& direction, float distance)
{
	glm::vec3 p = m_defaultPosition + direction * distance;
	move(p);
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
	p = ClosestPtPointTriangle(s.center(), a, b, c);

	// Sphere and triangle intersect if the (squared) distance from sphere 
	// center to point p is less than the (squared) sphere radius 
	glm::vec3 v = p - s.center();
	return glm::dot(v, v) <= s.radius() * s.radius();
}

/**
 * Test collision againsta sphere
 */
bool dfMesh::collide(glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection, std::string& name)
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

					std::cerr << "dfMesh::collide ray collide" << std::endl;
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

	if (m_boundingBox.intersect(aabb)) {
		// now test with the sphere against each triangle
		for (unsigned int i = 0; i < m_vertices.size(); i += 3) {
			if (TestSphereTriangle(bsTranformed, m_vertices[i], m_vertices[i + 1], m_vertices[i + 2], intersection)) {
				// convert back the collision point from model space => world space
				bsTranformed.center(intersection);
				bsTranformed.applyMatrix4(m_mesh->worldMatrix());
				intersection = bsTranformed.center();

				glm::vec3 hit = intersection - position;
				direction = target - position;
				if (glm::dot(hit, direction) > 0) {
					// as we checked on a sphere, ensure the intersection is in the direction we want to move, and not on our back
					std::cerr << "dfMesh::collide sphere collide" << std::endl;
					return true;
				}
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
	return m_mesh->is_visible() && m_parent->is_visible();
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

	rebuildAABB();
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
	m_mesh->translate(m_position);

	return m_mesh;
}

dfMesh::~dfMesh()
{
	delete m_geometry;
	delete m_mesh;
}
