#include "dfSuperSector.h"

#include <algorithm>
#include <array>
#include <glm/glm.hpp> 
#include "../include/earcut.hpp"

#include "../framework/geometries/fwGeometrySphere.h"

#include "../gaEngine/gaWorld.h"

#include "dfLevel.h"
#include "dfSign.h"
#include "dfMesh.h"

static glm::vec4 white(1.0, 0.0, 1.0, 1.0);
static fwMaterialBasic* material_portal = new fwMaterialBasic(&white);

dfSuperSector::dfSuperSector(dfSector* sector, fwMaterialBasic* material, std::vector<dfBitmap*>& bitmaps) :
	m_id(nbSuperSectors++),
	m_worldAABB(sector->m_worldAABB),
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
	return m_worldAABB.surface();
}

/**
 * merge a super-sector into this one
 */
void dfSuperSector::extend(dfSuperSector* ssector)
{
	// extend the bounding box
	m_worldAABB.extend(ssector->m_worldAABB);

	// extend the list of sectors
	m_sectors.insert(m_sectors.end(), ssector->m_sectors.begin(), ssector->m_sectors.end());

	// drop the portals between the 2 super sectors
	m_portals.remove_if([ssector](dfPortal& portal) { return portal.m_target == ssector;});
	ssector->m_portals.remove_if([this](dfPortal& portal) { return portal.m_target == this;});

	// parse all portals from the super-sector. 
	// It will be removed => adjoint super-sectors have to rebound to point to the new super-sector
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
	m_worldAABB.extend(box);
}

/**
 * check if point is inside the AABB
 */
bool dfSuperSector::isPointInside(const glm::vec3& position)
{
	return m_worldAABB.inside(position);
}

/**
 * quick AABB check for entities collision
 */
bool dfSuperSector::collideAABB(const fwAABBox& box)
{
	return m_worldAABB.intersect(box);
}

/**
 * extended segment collision test after a successful AABB collision
 */
bool dfSuperSector::collisionSegmentTriangle(const glm::vec3& p, const glm::vec3& q, std::list<gaCollisionPoint>& collisions)
{
	if (m_dfmesh) {
		m_dfmesh->collisionSegmentTriangle(p, q, collisions);
	}
	return false;
}

/**
 * parse all portals to find the smalled adjacent
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
 * test if a super-sector contains a specific sector
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
dfSector* dfSuperSector::findSector(const glm::vec3& position)
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
 * Create the geometry
 */
void dfSuperSector::buildGeometry(std::vector<dfSector*>& sectors)
{
	for (auto sector : m_sectors) {
		sector->buildGeometry(m_dfmesh, dfWallFlag::NOT_MORPHS_WITH_ELEV);

		// buildSigns(sector, sectors);
	}
	m_dfmesh->name(m_name);
	m_dfmesh->buildMesh();

	// TODO : fix the camera frustum test to remove that line
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

std::vector<dfBitmap*>& dfSuperSector::textures(void)
{
	return m_parent->textures();
}

/**
 * test all portals of the super-sector against the camera frustum.
 * for any visible portal, add the attached super-sector to the list of visible super-sectors
 * and test the portals again. Avoid coming back to already visited super-sectors
 */
void dfSuperSector::checkPortals(fwCamera* camera, int zOrder)
{
	m_dfmesh->zOrder(zOrder);

	for (auto& portal : m_portals) {
		// WARNING : the camera is using opengl space, but the boundSphere are translated to opengl space
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
 * create a full hierarchy
 */
void dfSuperSector::buildHiearchy(dfLevel* parent)
{
	m_parent = parent;

	for (auto sector : m_sectors) {
		sector->parent(this);
	}
}

/**
 * parse the scene the find the super-sector and change its visibility
 * if the supers-sector is not on the scene and is visible => add to the scene
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

	// take the opportunity to create a name for the super sector
	for (auto sector : m_sectors) {
		m_name += sector->m_name + ";";
	}
}

/**
 * Update the ambient light in the dfMesh
 */
void dfSuperSector::updateAmbientLight(float ambient, int start, int len)
{
	m_dfmesh->changeAmbient(ambient, start, len);
}


dfSuperSector::~dfSuperSector()
{
	delete m_geometry;
	delete m_mesh;

	for (auto sign : m_hSigns) {
		delete sign.second;
	}
}