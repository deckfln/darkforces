#include "dfCollision.h"

dfCollision::dfCollision()
{
}

/**
 * Return the altitude of the floor at the player position
 */
float dfCollision::ground(glm::vec3& position)
{
	assert(m_level != nullptr);

	// find the sector of the position
	dfSector* currentSector = m_level->findSector(position);
	if (currentSector) {
		//std::cout << currentSector->m_id << " " << currentSector->m_floorAltitude << std::endl;
		// TODO where should the conversion from level space to opengl space take place
		return currentSector->m_floorAltitude / 10.f;
	}

	return 0;
}

/**
 * test if there is a wall in front of the player
 */
bool dfCollision::checkEnvironement(glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection)
{
	return m_level->checkCollision(3, position, target, radius, intersection);
}

dfCollision::~dfCollision(void)
{
}
