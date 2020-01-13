#include "dfLogicTrigger.h"

#include "dfLogicTrigger.h"
#include "dfSector.h"

/**
 * Create a trigger without bounding box
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex) :
	m_class(kind),
	m_sector(sector),
	m_wallIndex(wallIndex)
{

}

/**
 * Create a trigger based on a wall of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex,  dfLogicElevator* client) :
	m_class(kind),
	m_wallIndex(wallIndex)
{
	m_pClients.push_back(client);

	sector->setTriggerFromWall(this);
}

/**
 * Create a trigger based on the floor of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client) :
	m_class(kind)
{
	m_pClients.push_back(client);

	sector->setTriggerFromFloor(this);
}

void dfLogicTrigger::boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling)
{
	m_boundingBox = fwAABBox(
		left.x, right.x,
		left.y, right.y,
		floor, ceiling
	);

	m_boundingBoxCenter.x = (left.x + right.x) / 2.0f;
	m_boundingBoxCenter.y = (left.y + right.y) / 2.0f;
	m_boundingBoxCenter.z = (floor + ceiling) / 2.0f;

	m_boundingBoxSize.x = abs(left.x - right.x);
	m_boundingBoxSize.y = abs(left.y - right.y);
	m_boundingBoxSize.z = abs(ceiling - floor);
}

bool dfLogicTrigger::collide(fwAABBox& box)
{
	return m_boundingBox.intersect(box);
}

/**
 * move the boundingBox of the trigger
 */
void dfLogicTrigger::moveZ(float z)
{
	m_boundingBox.m_z = z;
	m_boundingBox.m_z1 = z + m_boundingBoxSize.z;
}

void dfLogicTrigger::activate(void)
{
	for (auto pClient: m_pClients) {
		pClient->trigger(m_class);
	}
}
