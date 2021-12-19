#include "gaLevel.h"

#include <vector>
#include "../framework/fwCamera.h"
#include "../darkforces/dfSuperSector.h"

GameEngine::Level* g_gaLevel = nullptr;

GameEngine::Level::Level()
{
}

/**
 * parse the super sectors to find which one are in the camera frustum
 * use the portals to drill through portals
 */
void GameEngine::Level::draw(dfSuperSector* current, fwCamera* camera)
{
	// ignore the whole process if the camera didn't change
	if (camera->worldMatrix() == m_cachedCameraMatrix)
	{
		return;
	}
	m_cachedCameraMatrix = camera->worldMatrix();

	// find what sector changed visibility
	std::vector<bool> visibilityCache(m_supersectors.size());

	// mark all super sectors as NO VISBILE
	auto i = 0;
	for (auto ssector : m_supersectors) {
		visibilityCache[i++] = ssector->visible();
		ssector->visible(false);
	}

	if (current) {
		current->visible(true);

		// recursively test the portals to make super sectors visible in the camera frustum 
		current->checkPortals(camera, 1);
	}
	else {
		// ELSE outside of the map => not good
		return;
	}

	// inform each sector of the change of visibility
	i = 0;
	for (auto ssector : m_supersectors) {
		if (visibilityCache[i] != ssector->visible()) {
			if (ssector->visible()) {
				ssector->sendMessage(gaMessage::Action::UNHIDE);
			}
			else {
				ssector->sendMessage(gaMessage::Action::HIDE);
			}
		}
		i++;
	}
}

/**
 * hide all sectors and inform them
 */
void GameEngine::Level::hideSectors(void)
{
	for (auto ssector : m_supersectors) {
		ssector->visible(false);
		ssector->sendMessage(gaMessage::Action::HIDE);
	}
}

/**
 * return the sector the point is in
 */
dfSuperSector* GameEngine::Level::findSector(const glm::vec3& position)
{
	// use the cached values to find the current super sector
	if (m_lastSuperSector && m_lastSuperSector->isPointInside(position)) {
		return m_lastSuperSector;
	}

	for (auto sector : m_supersectors) {
		if (sector->isPointInside(position)) {
			m_lastSuperSector = sector;
			return sector;
		}
	}
	return nullptr;
}

GameEngine::Level::~Level()
{
}
