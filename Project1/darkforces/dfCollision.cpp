#include "dfCollision.h"

//maximum step the player can walk over (glSpace)
#define DF_MAXIMUM_STEP 0.26f

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
	dfSector* currentSector = m_level->findSector(position, nullptr);
	if (currentSector) {
		//std::cout << currentSector->m_id << " " << currentSector->m_floorAltitude << std::endl;
		// TODO where should the conversion from level space to opengl space take place
		return currentSector->currentFloorAltitude() / 10.f;
	}

	return 0;
}


dfCollision::~dfCollision(void)
{
}
