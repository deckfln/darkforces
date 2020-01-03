#include "dfSuperSector.h"

#include <algorithm>

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
 * parse the walls and build a list of portals in level space
 */
void dfSuperSector::buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*>& vssectors)
{
	dfSector* root = m_sectors.front();	// only one sector in the list

	for (auto wall : root->m_walls) {
		if (wall->m_adjoint >= 0) {
			// this wall is a portal
			dfSector* target = sectors[wall->m_adjoint];

			// evaluate the portal size
			float floor = std::max(root->m_floorAltitude, target->m_floorAltitude);
			float ceiling = std::min(root->m_ceilingAltitude, target->m_ceilingAltitude);
			glm::vec2 left = root->m_vertices[wall->m_left];
			glm::vec2 right = root->m_vertices[wall->m_right];

			glm::vec3 center = glm::vec3(
				(left.x + right.x) / 2.0,
				(left.y + right.y) / 2.0,
				(floor + ceiling) / 2.0);

			float dx = pow(right.x - left.x, 2);
			float dy = pow(right.y - left.y, 2);
			float dz = pow(ceiling - floor, 2);
			float radius = sqrt(dx + dy + dz);
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
