#include "dfComponentActor.h"

#include "../../config.h"

#include "../../gaEngine/gaWorld.h"

#include "../dfObject/dfSpriteAnimated.h"
#include "../dfObject.h"
#include "../dfBullet.h"

dfComponentActor::dfComponentActor(void):
	gaComponent(DF_COMPONENT_ACTOR)
{
}

/**
 * Increase or decrease the shield and update the HUD
 */
void dfComponentActor::addShield(int value)
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
void dfComponentActor::addEnergy(int value)
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
 * hit by a bullet, reduce shield and life
 */
void dfComponentActor::hitBullet(int value)
{
	if (m_shield > 0) {
		m_shield -= value;
	}
	else {
		m_life -= value;
		if (m_life < 0) {
			die();
		}
	}
}

/**
 * Deal with messages
 */
void dfComponentActor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_ADD_SHIELD:
		addShield(message->m_value);
		break;
	case DF_MESSAGE_ADD_ENERGY:
		addEnergy(message->m_value);
		break;
	case DF_MESSAGE_HIT_BULLET:
		hitBullet(message->m_value);
		break;
	}
}

/**
 * fire the current weapon
 */
void dfComponentActor::fire(const glm::vec3& direction)
{
	// create a bullet
	// and add to the world to live its life
	dfBullet* bullet = new dfBullet(m_parent->position() + glm::vec3(0, 0.3, 0), direction);

	g_gaWorld.addClient(bullet);

	gaDebugLog(1, "dfActor::fire", "bullet");
}

/**
 * kill the actor
 */
void dfComponentActor::die(void)
{
	// inform the world it can remove the entity from its list
	//g_gaWorld.sendMessage(m_parent->name(), "_world", GA_MSG_DELETE_ENTITY, 0, nullptr);
	((dfSpriteAnimated*)m_parent)->state(DF_STATE_ENEMY_DIE_FROM_SHOT);

	gaDebugLog(1, "dfActor::die", "remove " + m_parent->name() + " the entity from the world");
}

dfComponentActor::~dfComponentActor()
{
}
