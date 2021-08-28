#include "gaLevel.h"

#include "../framework/fwCamera.h"
#include "../darkforces/dfSuperSector.h"

GameEngine::Level::Level()
{
}

/**
 * parse the super sectors to find which one are in the camera frustum
 * use the portals to drill through portals
 */
void GameEngine::Level::draw(const glm::vec3& position, fwCamera* camera)
{
	// mark all super sectors as NO VISBILE
	for (auto ssector : m_supersectors) {
		ssector->visible(false);
	}

	dfSuperSector* current = findSector(position);
	if (current) {
		current->visible(true);

		// recursively test the portals to make super sectors visible in the camera frustum 
		current->checkPortals(camera, 1);
	}
	// ELSE outside of the map
}

/**
 * return the sector the point is in
 */
dfSuperSector* GameEngine::Level::findSector(const glm::vec3& position)
{
	// use the cached values to find the current super sector
	if (m_lastSuperSector && m_lastSuperSector->inAABBox(position)) {
		return m_lastSuperSector;
	}

	for (auto sector : m_supersectors) {
		if (sector->inAABBox(position)) {
			m_lastSuperSector = sector;
			return sector;
		}
	}
	return nullptr;
}

GameEngine::Level::~Level()
{
}
