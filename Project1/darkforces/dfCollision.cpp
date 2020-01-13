#include "dfCollision.h"

dfCollision::dfCollision(void)
{
}

float dfCollision::ground(glm::vec3& position)
{
	assert(m_level != nullptr);

	// find the sector of the position
	dfSector* currentSector = m_level->findSector(position);
	if (currentSector) {
		//std::cout << currentSector->m_id << " " << currentSector->m_floorAltitude << std::endl;
		// TODO where should the conversion from level space to opengl space take place
		return currentSector->m_floorAltitude / 10;
	}

	return 0;
}

dfCollision::~dfCollision(void)
{
}
