#include "dfComponentActor.h"

#include "../../config.h"

#include "../../gaEngine/World.h"

#include "../dfObject/dfSpriteAnimated.h"
#include "../dfObject.h"
#include "../dfBullet.h"
#include "../../gaEngine/gaActor.h"

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
	case DF_MSG_PICK_RIFLE_AND_BULLETS:
		addEnergy(message->m_value);
		//TODO add a weapon to a player
		//addWeapon(O_RIFLE);
	}
}

/**
 * fire the current weapon
 */
void dfComponentActor::fire(const glm::vec3& direction)
{
	// create a bullet
	// and add to the world to live its life
	gaActor* actor = static_cast<gaActor*>(m_entity);
	glm::vec3 p = m_entity->position();
	p.y += actor->height() / 2.0f;

	dfBullet* bullet = new dfBullet(p + direction * actor->radius(), direction);

	g_gaWorld.addClient(bullet);

	gaDebugLog(1, "dfActor::fire", "bullet");
}

/**
 * kill the actor
 */
void dfComponentActor::die(void)
{
	// inform the world it can remove the entity from its list
	//g_gaWorld.sendMessage(m_parent->name(), "_world", gaMessage::DELETE_ENTITY, 0, nullptr);
	((dfSpriteAnimated*)m_entity)->state(dfState::ENEMY_DIE_FROM_SHOT);

	// object can now be traversed
	((dfSpriteAnimated*)m_entity)->hasCollider(false);

	gaDebugLog(1, "dfActor::die", "remove " + m_entity->name() + " the entity from the world");
}

dfComponentActor::~dfComponentActor()
{
}
