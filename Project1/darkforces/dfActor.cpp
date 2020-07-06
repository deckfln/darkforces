#include "dfActor.h"

#include "../config.h"

#include "dfObject.h"

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

dfActor::~dfActor()
{
}
