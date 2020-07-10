#include "dfActor.h"

#include "../config.h"

#include "../gaEngine/gaWorld.h"

#include "dfObject.h"
#include "dfBullet.h"

dfActor::dfActor(const std::string& name, fwCylinder& bounding, float eyes, float ankle):
	gaActor(DF_ENTITY_ACTOR, name, bounding, eyes, ankle)
{
}

/**
 * Increase or decrease the shield and update the HUD
 */
void dfActor::addShield(int value)
{
	m_shield += value;
	if (m_shield > m_maxShield) {
		m_shield = m_maxShield;
	}
	else if (m_shield < 0) {
		m_shield = 0;
	}
}

/**
 * Increase or decrease the energy and update the HUD
 */
void dfActor::addEnergy(int value)
{
	m_energy += value;
	if (m_energy > m_maxEnergy) {
		m_energy = m_maxEnergy;
	}
	else if (m_energy < 0) {
		m_energy = 0;
	}
}

/**
 * Deal with messages
 */
void dfActor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_ADD_SHIELD:
		addShield(message->m_value);
		break;
	case DF_MESSAGE_ADD_ENERGY:
		addEnergy(message->m_value);
		break;
	}
}

/**
 * fire the current weapon
 */
void dfActor::fire(const glm::vec3& direction)
{
	// create a bullet
	dfBullet* bullet = new dfBullet(m_position + glm::vec3(0, 0.3, 0), direction);

	// and add to the world to live its life
	g_gaWorld.add2scene(bullet->mesh());

	gaDebugLog(1, "dfActor::fire", "bullet");
}

dfActor::~dfActor()
{
}
