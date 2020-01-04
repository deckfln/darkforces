#include "dfSuperSector.h"

#include <algorithm>
#include <array>

#include "../include/earcut.hpp"
#include "../framework/geometries/fwGeometrySphere.h"

static glm::vec4 white(1.0, 0.0, 1.0, 1.0);
static fwMaterialBasic* material_portal = new fwMaterialBasic(&white);

dfSuperSector::dfSuperSector(dfSector* sector)
{
	m_id = nbSuperSectors++;

	m_boundingBox = sector->m_boundingBox;
	m_sectors.push_back(sector);
}

/**
 * return the surface of the sector bounding box
 */
float dfSuperSector::boundingBoxSurface(void)
{
	return m_boundingBox.surface();
}

/**
 * merge a supersector into this one
 */
void dfSuperSector::extend(dfSuperSector* ssector)
{
	// extend the bounding box
	m_boundingBox.extend(ssector->m_boundingBox);

	// extend the list of sectors
	m_sectors.insert(m_sectors.end(), ssector->m_sectors.begin(), ssector->m_sectors.end());

	// drop the portals between the 2 super sectors
	m_portals.remove_if([ssector](dfPortal& portal) { return portal.m_target == ssector;});
	ssector->m_portals.remove_if([this](dfPortal& portal) { return portal.m_target == this;});

	// parse all portals from the sssector. 
	// It will be removed => adjoint supersectors have to reboud to point to the new supersector
	for (auto &portal : ssector->m_portals) {
		for (auto &remote_portal : portal.m_target->m_portals) {
			if (remote_portal.m_target == ssector) {
				remote_portal.m_target = this;
			}
		}
	}

	// extend the list of portals
	m_portals.insert(m_portals.end(), ssector->m_portals.begin(), ssector->m_portals.end());
}

/**
 * parse all portals to find the smalled adjent
 */
dfSuperSector* dfSuperSector::smallestAdjoint(void)
{
	float surface, min_surface = 99999;
	dfSuperSector* smallest = nullptr;
	for (auto portal : m_portals) {
		surface = portal.m_target->boundingBoxSurface();
		if (surface < min_surface) {
			min_surface = surface;
			smallest = portal.m_target;
		}
	}
	return smallest;
}

/**
 * Convert a position from level space to opengl space
 */
static glm::vec3 level2gl(glm::vec3 &levelSpace) {
	glm::vec3 a;

	a.x = levelSpace.x / (float)10.0;
	a.y = levelSpace.z / (float)10.0;
	a.z = levelSpace.y / (float)10.0;

	return a;
}

/**
 * parse the walls and build a list of portals in level space
 */
void dfSuperSector::buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*>& vssectors)
{
	dfSector* root = m_sectors.front();	// only one sector in the list

	for (auto wall : root->m_walls) {
		if (wall->m_adjoint >= 0) {
			// this wall is a portal
			dfSector* target = sectors[wall->m_adjoint];

			// portal size in level space
			float floor = std::max(root->m_floorAltitude, target->m_floorAltitude);
			float ceiling = std::min(root->m_ceilingAltitude, target->m_ceilingAltitude);
			glm::vec3 bottomLeft(
				root->m_vertices[wall->m_left].x,
				root->m_vertices[wall->m_left].y,
				floor);
			glm::vec3 topRight(
				root->m_vertices[wall->m_right].x,
				root->m_vertices[wall->m_right].y,
				ceiling);

			// and now to opengl
			topRight = level2gl(topRight);
			bottomLeft = level2gl(bottomLeft);

			glm::vec3 center = topRight + bottomLeft;
			center /= 2.0;

			float dx = pow(topRight.x - bottomLeft.x, 2);
			float dy = pow(topRight.y - bottomLeft.y, 2);
			float dz = pow(topRight.z - bottomLeft.z, 2);
			float radius = sqrt(dx + dy + dz) / 2.0;
			fwSphere boundingSphere = fwSphere(center, radius);

			m_portals.push_back(dfPortal(boundingSphere, vssectors[wall->m_adjoint]));
		}
	}
}

/**
 * test if a supersector contains a specific sector
 */
bool dfSuperSector::contains(int sectorID)
{
	for (auto sector : m_sectors) {
		if (sector->m_id == sectorID) {
			return true;
		}
	}
	return false;
}
/**
 * return the sector fitting the position
 */
dfSector* dfSuperSector::findSector(glm::vec3& position)
{
	// position is in level space
	if (inAABBox(position)) {
		for (auto sector : m_sectors) {
			if (sector->isPointInside(position)) {
				return sector;
			}
		}
	}

	return nullptr;	// not here
}

/***
 * create vertices for a rectangle
 */
void dfSuperSector::addRectangle(dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfTexture*>& textures)
{
	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	int textureID = wall->m_tex[texture].x;

	dfTexture* dfTexture = textures[textureID];

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = dfTexture->width;
	float ypixel = dfTexture->height;

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	float height = abs(z1 - z) * 8.0 / ypixel;
	float width = length * 8.0 / xpixel;

	// get local texture offset on the wall
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
	float xoffset = (wall->m_tex[texture].y * 8) / xpixel;
	float yoffset = (wall->m_tex[texture].z * 8) / ypixel;

	// resize the opengl buffers
	int p = m_vertices.size();
	m_vertices.resize(p + 6);
	m_uvs.resize(p + 6);
	m_textureID.resize(p + 6);

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
 * Convert a level into a mega textured mesh
 */
void dfSuperSector::buildWalls(std::vector<dfTexture*>& textures, std::vector<dfSector*>& sectors)
{
	int size = 0;
	int p = 0;

	for (auto sector : m_sectors) {
		for (auto wall : sector->m_walls) {
			if (wall->m_adjoint < 0) {
				// full wall
				addRectangle(sector, wall,
					sector->m_floorAltitude,
					sector->m_ceilingAltitude,
					DFWALL_TEXTURE_MID,
					textures
				);
			}
			else {
				// portal
				dfSector* portal = sectors[wall->m_adjoint];

				if (portal->m_ceilingAltitude < sector->m_ceilingAltitude) {
					// add a wall above the portal
					addRectangle(sector, wall,
						portal->m_ceilingAltitude,
						sector->m_ceilingAltitude,
						DFWALL_TEXTURE_TOP,
						textures
					);
				}
				if (portal->m_floorAltitude > sector->m_floorAltitude) {
					// add a wall below the portal
					addRectangle(sector, wall,
						sector->m_floorAltitude,
						portal->m_floorAltitude,
						DFWALL_TEXTURE_BOTTOM,
						textures
					);
				}
			}
		}
	}
}

/**
 * build the floor geometry by triangulating the shape
 * apply texture by using an axis aligned 8x8 grid
 */
void dfSuperSector::buildFloor(std::vector<dfTexture*>& textures)
{
	for (auto sector : m_sectors) {

		// The number type to use for tessellation
		using Coord = float;

		// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
		// data won't have more than 65536 vertices.
		using N = uint32_t;

		// Create array
		using Point = std::array<Coord, 2>;
		std::vector<std::vector<Point>> polygon;

		// Fill polygon structure with actual data. Any winding order works.
		// The first polyline defines the main polygon.
		// Following polylines define holes.
		polygon.resize(2);
		polygon[0].resize(sector->m_vertices.size());

		for (int i = 0; i < sector->m_vertices.size(); i++) {
			polygon[0][i] = { sector->m_vertices[i].x, sector->m_vertices[i].y };
		}

		// Run tessellation
		// Returns array of indices that refer to the vertices of the input polygon.
		// e.g: the index 6 would refer to {25, 75} in this example.
		// Three subsequent indices form a triangle. Output triangles are clockwise.
		std::vector<N> indices = mapbox::earcut<N>(polygon);

		// resize the opengl buffers
		int p = m_vertices.size();
		int vertices = indices.size() * 2;	// count the floor AND the ceiling
		m_vertices.resize(p + vertices);
		m_uvs.resize(p + vertices);
		m_textureID.resize(p + vertices);

		// use axis aligned texture UV, on a 8x8 grid
		// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
		dfTexture* dfTexture = textures[sector->m_floorTexture.r];
		float xpixel = dfTexture->width;
		float ypixel = dfTexture->height;

		// warning, triangles are looking downward
		int currentVertice = 0, j;
		for (auto i = 0; i < indices.size(); i++) {
			int index = indices[i];

			// reverse vertices 2 and 3 to look upward
			switch (currentVertice) {
			case 1: j = 1; break;
			case 2: j = -1; break;
			default: j = 0; break;
			}
			m_vertices[p + j].x = sector->m_vertices[index].x / 10.0;
			m_vertices[p + j].y = sector->m_floorAltitude / 10.0;
			m_vertices[p + j].z = sector->m_vertices[index].y / 10.0;

			// get local texture offset on the floor
			// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
			float xoffset = ((sector->m_vertices[index].x + sector->m_floorTexture.g) * 8) / xpixel;
			float yoffset = ((sector->m_vertices[index].y + sector->m_floorTexture.b) * 8) / ypixel;

			m_uvs[p + j] = glm::vec2(xoffset, yoffset);

			m_textureID[p + j] = sector->m_floorTexture.r;

			p++;
			currentVertice = (currentVertice + 1) % 3;
		}

		// use axis aligned texture UV, on a 8x8 grid
		// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
		dfTexture = textures[sector->m_ceilingTexture.r];
		xpixel = dfTexture->width;
		ypixel = dfTexture->height;

		// create the ceiling
		for (auto i = 0; i < indices.size(); i++) {
			int index = indices[i];

			m_vertices[p].x = sector->m_vertices[index].x / 10.0;
			m_vertices[p].y = sector->m_ceilingAltitude / 10.0;
			m_vertices[p].z = sector->m_vertices[index].y / 10.0;

			// use axis aligned texture UV, on a 8x8 grid
			// ratio of texture pixel vs world position = 64 pixels for 8 clicks
			float xoffset = ((sector->m_vertices[index].x + sector->m_ceilingTexture.g) * 8) / xpixel;
			float yoffset = ((sector->m_vertices[index].y + sector->m_ceilingTexture.g) * 8) / ypixel;

			m_uvs[p] = glm::vec2(xoffset, yoffset);

			m_textureID[p] = sector->m_ceilingTexture.r;

			p++;
		}

	}
}

/**
 * Create the geometry
 */
void dfSuperSector::buildGeometry(std::vector<dfSector*>& sectors, std::vector<dfTexture*>& textures, fwMaterialBasic* material)
{
	buildWalls(textures, sectors);
	buildFloor(textures);

	int size = m_vertices.size();
	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", &m_vertices[0], 3, size * sizeof(glm::vec3), sizeof(float), false);
	m_geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, &m_uvs[0], 2, size * sizeof(glm::vec2), sizeof(float), false);
	m_geometry->addAttribute("aTextureID", GL_ARRAY_BUFFER, &m_textureID[0], 1, size * sizeof(float), sizeof(float), false);

	m_mesh = new fwMesh(m_geometry, material);
	// TODO : fix the camera frustrum test to remove that line
	// m_mesh->always_draw(true);

	if (m_debugPortals) {
		// add the portals on screen
		for (auto& portal : m_portals) {
			fwGeometrySphere* sphere = new fwGeometrySphere(portal.m_boundingSphere.radius());
			portal.m_debug_portal = new fwMesh(sphere, material_portal);
			glm::vec3 position = portal.m_boundingSphere.center();

			portal.m_debug_portal->position(position);

			m_mesh->addChild(portal.m_debug_portal);
		}
	}
}

/**
 * test all portals of the supersector agsint the camera frustrum.
 * for any visible portal, add the attached supersector to the list of visible supersectors
 * and test the portals again. Avoid coming back to already visited supersectors
 */
void dfSuperSector::checkPortals(fwCamera* camera)
{
	for (auto& portal : m_portals) {
		// WARNING : the camera is using opengl space, but the boundSphere are translated to gl space
		if (camera->is_inFrustum(portal.m_boundingSphere)) {
			dfSuperSector* target = portal.m_target;
			if (!target->m_visible) {
				target->m_visible = true;
				target->checkPortals(camera);
			}
		}
	}
}

/**
 * parse the scene the find the supersector and change its visibility
 * if the superssector is not on the scene and is visible => add to the scene
 */
void dfSuperSector::add2scene(fwScene* scene)
{
	if (!scene->hasChild(m_mesh)) {
		if (m_visible) {
			// add the mesh on the scene
			scene->addChild(m_mesh);
			m_mesh->set_visible(true);
		}
		// no need to add the msh if the supersector is invisible
	}
	else {
		m_mesh->set_visible(m_visible);
	}
}

dfSuperSector::~dfSuperSector()
{
	delete m_geometry;
	delete m_mesh;
}