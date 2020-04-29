#include "dfSuperSector.h"

#include <algorithm>
#include <array>
#include <glm/glm.hpp> 
#include "../include/earcut.hpp"
#include "../framework/geometries/fwGeometrySphere.h"

#include "dfLevel.h"
#include "dfSign.h"
#include "dfMessageBus.h"
#include "dfMesh.h"

static glm::vec4 white(1.0, 0.0, 1.0, 1.0);
static fwMaterialBasic* material_portal = new fwMaterialBasic(&white);

dfSuperSector::dfSuperSector(dfSector* sector, fwMaterialBasic* material, std::vector<dfBitmap*>& bitmaps) :
	m_id(nbSuperSectors++),
	m_boundingBox(sector->m_boundingBox),
	m_material(material)
{
	m_sectors.push_back(sector);

	m_dfmesh = new dfMesh(material, bitmaps);
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
 * Extend the AABBox
 */
void dfSuperSector::extendAABB(fwAABBox& box)
{
	m_boundingBox.extend(box);
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
			float floor = std::max(root->staticFloorAltitude(), target->staticFloorAltitude());
			float ceiling = std::min(root->staticCeilingAltitude(), target->staticCeilingAltitude());
			glm::vec3 bottomLeft(
				root->m_vertices[wall->m_left].x,
				root->m_vertices[wall->m_left].y,
				floor);
			glm::vec3 bottomRight(
				root->m_vertices[wall->m_right].x,
				root->m_vertices[wall->m_right].y,
				floor);
			glm::vec3 topRight(
				root->m_vertices[wall->m_right].x,
				root->m_vertices[wall->m_right].y,
				ceiling);

			// and now to opengl
			topRight = level2gl(topRight);
			bottomLeft = level2gl(bottomLeft);
			bottomRight = level2gl(bottomRight);

			glm::vec3 normal = glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight));

			glm::vec3 center = topRight + bottomLeft;
			center /= 2.0;

			float dx = pow(topRight.x - bottomLeft.x, 2);
			float dy = pow(topRight.y - bottomLeft.y, 2);
			float dz = pow(topRight.z - bottomLeft.z, 2);
			float radius = sqrt(dx + dy + dz) / 2.0f;
			fwSphere boundingSphere = fwSphere(center, radius);

			m_portals.push_back(dfPortal(normal, boundingSphere, vssectors[wall->m_adjoint]));
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
			if (sector->isPointInside(position, false)) {
				return sector;
			}
		}
	}

	return nullptr;	// not here
}

/**
 * Update the vertices of a rectangle
 *
void dfSuperSector::updateRectangle(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, int textureID, float ambient)
{
	// TODO move conversion from level space to gl space in a dedicated function
	// first triangle
	m_vertices[p].x = x / 10;
	m_vertices[p].z = y / 10;
	m_vertices[p].y = z / 10;
	m_uvs[p] = glm::vec2(xoffset, yoffset);
	m_textureID[p] = (float)textureID;
	m_ambientLight[p] = ambient;

	m_vertices[p + 1].x = x1 / 10;
	m_vertices[p + 1].z = y1 / 10;
	m_vertices[p + 1].y = z / 10;
	m_uvs[p + 1] = glm::vec2(width + xoffset, yoffset);
	m_textureID[p + 1] = (float)textureID;
	m_ambientLight[p + 1] = ambient;

	m_vertices[p + 2].x = x1 / 10;
	m_vertices[p + 2].z = y1 / 10;
	m_vertices[p + 2].y = z1 / 10;
	m_uvs[p + 2] = glm::vec2(width + xoffset, height + yoffset);
	m_textureID[p + 2] = (float)textureID;
	m_ambientLight[p + 2] = ambient;

	// second triangle
	m_vertices[p + 3].x = x / 10;
	m_vertices[p + 3].z = y / 10;
	m_vertices[p + 3].y = z / 10;
	m_uvs[p + 3] = glm::vec2(xoffset, yoffset);
	m_textureID[p + 3] = (float)textureID;
	m_ambientLight[p + 3] = ambient;

	m_vertices[p + 4].x = x1 / 10;
	m_vertices[p + 4].z = y1 / 10;
	m_vertices[p + 4].y = z1 / 10;
	m_uvs[p + 4] = glm::vec2(width + xoffset, height + yoffset);
	m_textureID[p + 4] = (float)textureID;
	m_ambientLight[p + 4] = ambient;

	m_vertices[p + 5].x = x / 10;
	m_vertices[p + 5].z = y / 10;
	m_vertices[p + 5].y = z1 / 10;
	m_uvs[p + 5] = glm::vec2(xoffset, height + yoffset);
	m_textureID[p + 5] = (float)textureID;
	m_ambientLight[p + 5] = ambient;
}
*/

/***
 * create vertices for a rectangle
 *
int dfSuperSector::addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture)
{
	std::vector<dfBitmap*>& bitmaps = m_parent->textures();

	int p = start;

	if (start == -1) {
		// add a new rectangle
		p = m_vertices.size();
		m_vertices.resize(p + 6);
		m_uvs.resize(p + 6);
		m_textureID.resize(p + 6);
		m_ambientLight.resize(p + 6);
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

	// light value (0->31 => 0.0=>1.0)
	float ambient = sector->m_ambient / 32.0f;
	updateRectangle(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, image->m_textureID, ambient);

	if (start >= 0) {
		return 6;	// we updated the rectangle, move to the next rectangle
	}

	return 0;	// we added a new rectangle, keep the index at -1
}
*/

/**
 * create a simple opengl Rectangle
 *
void dfSuperSector::addRectangle(dfSector *sector, dfWall* wall, float z, float z1, glm::vec3& texture)
{
	std::vector<dfBitmap*>& bitmaps = m_parent->textures();

	// add a new rectangle
	int p = m_vertices.size();
	m_vertices.resize(p + 6);
	m_uvs.resize(p + 6);
	m_textureID.resize(p + 6);
	m_ambientLight.resize(p + 6);

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	// deal with the wall texture
	float bitmapID = texture.x;

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
	float xoffset = (texture.y * 8.0f) / xpixel;
	float yoffset = (texture.z * 8.0f) / ypixel;

	// light value (0->31 => 0=> 255)
	float ambient = sector->m_ambient / 32.0f;
	updateRectangle(p, x, y, z, x1, y1, z1, xoffset, yoffset, width, height, image->m_textureID, ambient);
}
*/

/***
 * create vertices for a sign

void dfSuperSector::addSign(dfSector* sector, dfWall* wall, float z, float z1, int texture)
{
	float bitmapID = wall->m_tex[DFWALL_TEXTURE_SIGN].r;
	std::vector<dfBitmap*>& bitmaps = m_parent->textures();
	dfBitmap* bitmap = bitmaps[(int)bitmapID];

	// record the sign on the wall
	std::string m_name = sector->m_name + "(" + std::to_string(wall->m_id) + ")";

	dfLogicTrigger* trigger = (dfLogicTrigger*)g_MessageBus.getClient(m_name);
	if (trigger) {
		dfSign* sign = new dfSign(this, &m_vertices, &m_uvs, &m_textureID, &m_ambientLight, bitmaps, bitmap, sector, wall, z, z1);
		trigger->sign(sign);
	}
}
*/

/**
 * Add walls at the begining of the vertices buffer
 *
void dfSuperSector::buildWalls(bool update, dfSector* sector, std::vector<dfSector *>sectors)
{
	int size = 0;
	int p = -1;
	if (update) {
		glm::ivec3 indexes = m_sectorIndex[sector->m_id];
		p = indexes.x;
	}

	int start = m_vertices.size();

	// create the walls at the begining of the buffer
	// Only create walls that do not move with the sector, the others are managed by an elevator
	for (auto wall : sector->walls(DF_WALL_NOT_MORPHS_WITH_ELEV)) {

		if (wall->m_adjoint < 0) {
			// full wall
			p += addRectangle(p, sector, wall,
				sector->m_floorAltitude,
				sector->m_ceilingAltitude,
				DFWALL_TEXTURE_MID
			);
		}
		else {
			// portal
			dfSector* portal = sectors[wall->m_adjoint];

			if (portal->m_ceilingAltitude < sector->m_ceilingAltitude) {
				// add a wall above the portal
				p += addRectangle(p, sector, wall,
					portal->m_ceilingAltitude,
					sector->m_ceilingAltitude,
					DFWALL_TEXTURE_TOP
				);
			}
			if (portal->m_floorAltitude > sector->m_floorAltitude) {
				// add a wall below the portal
				p += addRectangle(p, sector, wall,
					sector->m_floorAltitude,
					portal->m_floorAltitude,
					DFWALL_TEXTURE_BOTTOM
				);
			}
		}
	}

	// record the vertices for the walls
	sector->wallVertices(start, m_vertices.size() - start);
}
*/

/**
 * Create the signs at the end of the vertics buffer
 *
void dfSuperSector::buildSigns(dfSector*sector, std::vector<dfSector*>sectors)
{
	int size = 0;
	int p = 0;

	if (sector->m_name == "gigantaur_switch") {
		printf("dfSuperSector::buildSigns\n");
	}

	for (auto wall : sector->walls(DF_WALL_ALL)) {
		if (wall->m_tex[DFWALL_TEXTURE_SIGN].r >= 0) {

			if (wall->m_adjoint < 0) {
				// full wall
				addSign(sector, wall,
					sector->m_floorAltitude,
					sector->m_ceilingAltitude,
					DFWALL_TEXTURE_MID
				);
			}
			else {
				// portal
				dfSector* portal = sectors[wall->m_adjoint];
				int nbSigns = 0;

				if (portal->m_ceilingAltitude < sector->m_ceilingAltitude) {
					// add a wall above the portal
					addSign(sector, wall,
						portal->m_ceilingAltitude,
						sector->m_ceilingAltitude,
						DFWALL_TEXTURE_TOP
					);
					nbSigns++;
				}
				if (portal->m_floorAltitude > sector->m_floorAltitude) {
					// add a wall below the portal
					addSign(sector, wall,
						sector->m_floorAltitude,
						portal->m_floorAltitude,
						DFWALL_TEXTURE_BOTTOM
					);
					nbSigns++;
				}

				if (nbSigns == 0) {
					// force a sign on the wall because the portal is not visible
					addSign(sector, wall,
						sector->m_floorAltitude,
						sector->m_ceilingAltitude,
						DFWALL_TEXTURE_MID
						);
				}
			}
		}
	}
}
*/

/**
 * build the floor geometry by triangulating the shape
 * apply texture by using an axis aligned 8x8 grid
 *
void dfSuperSector::buildFloor(bool update, dfSector* sector)
{
	std::vector<dfBitmap*>& bitmaps = m_parent->textures();
	int start = m_vertices.size();

	if (update) {
		// ONLY update the vertices

		glm::ivec3 indexes = m_sectorIndex[sector->m_id];
		int nbvertices = indexes.z / 2;

		int start = indexes.y;
		for (auto i = 0; i < nbvertices; i++) {
			m_vertices[start + i].y = sector->m_floorAltitude / 10.0f;
		}

		start = indexes.y + nbvertices;
		for (auto i = 0; i < nbvertices; i++) {
			m_vertices[start + i].y = sector->m_ceilingAltitude/ 10.0f;
		}

		return;
	}

	// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
	// data won't have more than 65536 vertices.
	using N = uint32_t;

	// Create array

	// Fill polygon structure with actual data. Any winding order works.
	// The first polyline defines the main polygon.
	// Following polylines define holes.
	if (sector->m_id == 3 || sector->m_id == 176 || sector->m_id == 197 || sector->m_id == 60) {
		//TODO do not forget that 'thing'
		printf("dfSuperSector::buildFloor sector %d\n", sector->m_id);
	}
	std::vector<std::vector<Point>>& polygon = sector->polygons(-1);	// default polygons

	// index the indexes IN the polyines of polygon 
	std::vector<Point> vertices;

	for (auto poly: polygon) {
		for (auto p : poly) {
			vertices.push_back(p);
		}
	}

	// light value (0->31 => 0=> 255)
	float ambient = sector->m_ambient / 32.0f;

	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygon);

	// Create the ceiling, unless there is a sky

	// resize the opengl buffers
	int p = m_vertices.size();
	int cvertices = indices.size();	// count the floor 
	m_vertices.resize(p + cvertices);
	m_uvs.resize(p + cvertices);
	m_textureID.resize(p + cvertices);
	m_ambientLight.resize(p + cvertices);

	// use axis aligned texture UV, on a 8x8 grid
	// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
	dfBitmapImage* image = bitmaps[(int)sector->m_floorTexture.r]->getImage();
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

		// reverse vertices 2 and 3 to look upward
		switch (currentVertice) {
		case 1: j = 1; break;
		case 2: j = -1; break;
		default: j = 0; break;
		}

		m_vertices[p + j].x = vertices[index][0] / 10.0f;
		m_vertices[p + j].y = sector->m_floorAltitude / 10.0f;
		m_vertices[p + j].z = vertices[index][1] / 10.0f;

		// get local texture offset on the floor
		// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
		float xoffset = ((vertices[index][0] + sector->m_floorTexture.g) * 8.0f) / xpixel;
		float yoffset = ((vertices[index][1] + sector->m_floorTexture.b) * 8.0f) / ypixel;

		m_uvs[p + j] = glm::vec2(xoffset, yoffset);

		m_textureID[p + j] = (float)image->m_textureID;
		m_ambientLight[p + j] = ambient;

		p++;
		currentVertice = (currentVertice + 1) % 3;
	}

	// Create the ceiling, unless there is a sky
	if (!(sector->m_flag1 & DF_SECTOR_EXTERIOR_NO_CEIL)) {
		p = m_vertices.size();
		m_vertices.resize(p + cvertices);
		m_uvs.resize(p + cvertices);
		m_textureID.resize(p + cvertices);
		m_ambientLight.resize(p + cvertices);

		// use axis aligned texture UV, on a 8x8 grid
		// ratio of texture pixel vs world position = 180 pixels for 24 clicks = 7.5x1
		image = bitmaps[(int)sector->m_ceilingTexture.r]->getImage();
		xpixel = 0;
		ypixel = 0;
		if (image != nullptr) {
			xpixel = (float)image->m_width;
			ypixel = (float)image->m_height;
		}

		// create the ceiling
		for (unsigned int i = 0; i < indices.size(); i++) {
			int index = indices[i];

			m_vertices[p].x = vertices[index][0] / 10.0f;
			m_vertices[p].y = sector->m_ceilingAltitude / 10.0f;
			m_vertices[p].z = vertices[index][1] / 10.0f;

			// use axis aligned texture UV, on a 8x8 grid
			// ratio of texture pixel vs world position = 64 pixels for 8 clicks
			float xoffset = ((vertices[index][0] + sector->m_ceilingTexture.g) * 8.0f) / xpixel;
			float yoffset = ((vertices[index][1] + sector->m_ceilingTexture.g) * 8.0f) / ypixel;

			m_uvs[p] = glm::vec2(xoffset, yoffset);

			m_textureID[p] = (float)image->m_textureID;
			m_ambientLight[p] = ambient;

			p++;
		}
	}

	// record the vertices for the floor and ceiling
	sector->floorVertices(start, m_vertices.size() - start);
}
*/

/**
 * Create the geometry
 */
void dfSuperSector::buildGeometry(std::vector<dfSector*>& sectors)
{
	for (auto sector : m_sectors) {
		sector->buildGeometry(m_dfmesh, DF_WALL_NOT_MORPHS_WITH_ELEV);

		// buildSigns(sector, sectors);
	}
	m_dfmesh->buildMesh();

	// TODO : fix the camera frustrum test to remove that line
	// m_mesh->always_draw(true);

	if (m_debugPortals) {
		// add the portals on screen
		for (auto& portal : m_portals) {
			fwGeometrySphere* sphere = new fwGeometrySphere(portal.m_boundingSphere.radius());
			portal.m_debug_portal = new fwMesh(sphere, material_portal);
			glm::vec3 position = portal.m_boundingSphere.center();

			portal.m_debug_portal->translate(position);

			m_mesh->addChild(portal.m_debug_portal);
		}
	}
}

/**
 * Update the TextureIDs Attribute on the GPU
 *
void dfSuperSector::updateGeometryTextures(int start, int nb)
{
	m_geometry->updateAttribute("aTextureID", start, nb);
}
*/

/**
 * refresh all vertices of the sector
 *
void dfSuperSector::updateSectorVertices(int sectorID)
{
	std::vector<dfSector*>& sectors = m_parent->sectorsID();

	dfSector* sector = sectors[sectorID];

	buildWalls(true, sector, sectors);
	buildFloor(true, sector);

	m_geometry->update();
}
*/

std::vector<dfBitmap*>& dfSuperSector::textures(void)
{
	return m_parent->textures();
}

/**
 * test all portals of the supersector agsint the camera frustrum.
 * for any visible portal, add the attached supersector to the list of visible supersectors
 * and test the portals again. Avoid coming back to already visited supersectors
 */
void dfSuperSector::checkPortals(fwCamera* camera, int zOrder)
{
	m_dfmesh->zOrder(zOrder);

	for (auto& portal : m_portals) {
		// WARNING : the camera is using opengl space, but the boundSphere are translated to gl space
		if (camera->is_inFrustum(portal.m_boundingSphere)) {
			// only if the portal is looking outward
			glm::vec3 look2portal = glm::normalize(camera->get_position() - portal.m_boundingSphere.center());
			float d = glm::dot(look2portal, portal.m_normal);
			if (d >= 0) {
				dfSuperSector* target = portal.m_target;
				if (!target->m_visible) {
					target->m_visible = true;
					target->checkPortals(camera, zOrder + 1);
				}
			}
		}
	}
}

/**
 * create a full hiearchy
 */
void dfSuperSector::buildHiearchy(dfLevel* parent)
{
	m_parent = parent;

	for (auto sector : m_sectors) {
		sector->parent(this);
	}
}

/**
 * parse the scene the find the supersector and change its visibility
 * if the superssector is not on the scene and is visible => add to the scene
 */
void dfSuperSector::add2scene(fwScene* scene)
{
	m_dfmesh->display(scene, m_visible);
}

/**
 * Add a children mesh
 */
void dfSuperSector::addObject(dfMesh* object)
{
	m_dfmesh->addMesh(object->mesh());
	object->parent(m_mesh);
}

/**
 * Sort sectors by size. This is done for position testing. Sectors inside sectors will be tested first
 */
void dfSuperSector::sortSectors(void)
{
	m_sectors.sort([](dfSector* a, dfSector* b) { return a->boundingBoxSurface() < b->boundingBoxSurface(); });
}

/**
 * Update the AmbientLights attributes
 */
void dfSuperSector::updateAmbientLight(int start, int len)
{
	m_geometry->updateAttribute("aAmbient", start, len);
}

dfSuperSector::~dfSuperSector()
{
	delete m_geometry;
	delete m_mesh;

	for (auto sign : m_hSigns) {
		delete sign.second;
	}
}