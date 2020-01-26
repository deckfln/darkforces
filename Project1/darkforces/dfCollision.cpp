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
	dfSector* currentSector = m_level->findSector(position);
	if (!currentSector) {
		return false;
	}

	// convert GL space to DF space
	glm::vec3 dfPosition(target.x * 10.0f, target.z * 10.0f, target.y * 10.0f);
	glm::vec3 dfNew;

	if (currentSector->checkCollision(3, dfPosition, radius * 10.0f, dfNew)) {
		intersection.x = dfNew.x / 10.0f;
		intersection.y = dfNew.z / 10.0f;
		intersection.z = dfNew.y / 10.0f;
		return true;
	}

	return false;
}

dfCollision::~dfCollision(void)
{
}
