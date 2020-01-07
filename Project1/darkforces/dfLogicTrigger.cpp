#include "dfLogicTrigger.h"

#include "dfLogicTrigger.h"
#include "dfSector.h"

dfLogicTrigger::dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex) :
	m_class(kind),
	m_sector(sector),
	m_wallIndex(wallIndex)
{

}

void dfLogicTrigger::boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling)
{
	m_boundingBox = fwAABBox(
		left.x, right.x,
		left.y, right.y,
		floor, ceiling
	);
}

bool dfLogicTrigger::collide(fwAABBox& box)
{
	return m_boundingBox.intersect(box);
}

void dfLogicTrigger::activate(void)
{
	for (auto pClient: m_pClients) {
		pClient->trigger(m_class);
	}
}
