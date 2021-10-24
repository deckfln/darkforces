#include "dfComponentActor.h"

#include <imgui.h>

#include "../../config.h"

#include "../../gaEngine/World.h"

#include "../weapons.h"
#include "../dfObject/dfSpriteAnimated.h"
#include "../dfObject.h"
#include "../dfBullet.h"
#include "../../gaEngine/gaActor.h"

#include "../flightRecorder/frCompActor.h"

dfComponentActor::dfComponentActor(void):
	gaComponent(DF_COMPONENT_ACTOR)
{
}

/**
 * Increase or decrease the shield and update the HUD
 */
void dfComponentActor::addShield(int32_t value)
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
void dfComponentActor::addEnergy(int32_t value)
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
void dfComponentActor::hitBullet(int32_t value)
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
	case DarkForces::Message::ADD_SHIELD:
		addShield(message->m_value);
		break;
	case DarkForces::Message::ADD_ENERGY:
		addEnergy(message->m_value);
		break;
	case DarkForces::Message::HIT_BULLET:
		hitBullet(message->m_value);
		break;
	case DarkForces::Message::PICK_RIFLE_AND_BULLETS:
		addEnergy(message->m_value);
		//TODO add a weapon to a player
		//addWeapon(O_RIFLE);
		break;
	case DarkForces::Message::FIRE:
		fire(*(glm::vec3*)message->m_extra);
		break;
	case gaMessage::MOVE:
		// identity the DF sector and trigger enter/leave if changing
		if (message->m_extra == nullptr) {
			dfSector* current = m_level->findSector(m_entity->position());
			if (current != m_currentSector) {
				if (m_currentSector != nullptr) {
					m_entity->sendMessage(m_currentSector->name(), DarkForces::Message::EVENT, DarkForces::MessageEvent::LEAVE_SECTOR);

					if (current != nullptr) {
						m_entity->sendMessage(current->name(), DarkForces::Message::EVENT, DarkForces::MessageEvent::ENTER_SECTOR);
					}
				}
				m_currentSector = current;
			}
		}
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
	glm::vec3 p = m_entity->position();
	p.y += m_entity->height() / 2.0f;

	dfBullet* bullet = new dfBullet(DarkForces::Weapons::Rifle, p + direction * m_entity->radius() * 2.0f, direction);

	g_gaWorld.addClient(bullet);
#ifdef _DEBUG
	gaDebugLog(1, "dfActor::fire", "bullet");
#endif
}

/**
 * kill the actor
 */
void dfComponentActor::die(void)
{
	// inform the world it can remove the entity from its list
	((dfSpriteAnimated*)m_entity)->state(dfState::ENEMY_DIE_FROM_SHOT);

	// object can now be traversed
	m_entity->hasCollider(false);
	m_entity->physical(false);

	// play a sound if there is one
	m_entity->sendInternalMessage(gaMessage::PLAY_SOUND, 0);

	// cancel any move the actor was doing, for the next frame (gives a bit of latency)
	m_entity->sendDelayedMessage(gaMessage::Action::SatNav_CANCEL);

	gaDebugLog(1, "dfActor::die", "remove " + m_entity->name() + " the entity from the world");
}

/**
 * size of the component
 */
inline uint32_t dfComponentActor::recordSize(void)
{
	return sizeof(flightRecorder::DarkForces::CompActor);
}

/**
 * save the component state in a record
 */
uint32_t dfComponentActor::recordState(void* r)
{
	flightRecorder::DarkForces::CompActor* record = static_cast<flightRecorder::DarkForces::CompActor*>(r);
	record->size = sizeof(flightRecorder::DarkForces::CompActor);
	record->id = m_id;

	record->shield = m_shield;
	record->maxShield = m_maxShield;
	record->energy = m_energy;
	record->maxEnergy = m_maxEnergy;
	record->battery = m_battery;
	record->life = m_life;
	record->keys = static_cast<uint32_t>(m_keys);

	return record->size;
}

/**
 * reload a component state from a record
 */
uint32_t dfComponentActor::loadState(void* r)
{
	flightRecorder::DarkForces::CompActor* record = (flightRecorder::DarkForces::CompActor*)r;

	m_shield = record->shield;
	m_maxShield = record->maxShield;
	m_energy = record->energy;
	m_maxEnergy = record->maxEnergy;
	m_battery = record->battery;
	m_life = record->life;
	m_keys = static_cast<DarkForces::Keys>(record->keys);

	return record->size;
}

/**
 * display the component in the debugger
 */
void dfComponentActor::debugGUIinline(void)
{
	if (ImGui::TreeNode("dfComponentActor")) {
		if (m_currentSector) {
			ImGui::Text("Sector:%s", m_currentSector->name().c_str());
		}

		ImGui::Text("Shield: %d / %d", m_shield, m_maxShield);
		ImGui::Text("Energy: %d / %d", m_energy, m_maxEnergy);
		ImGui::Text("Battery: %d", m_battery);
		ImGui::Text("Life: %d", m_life);
		ImGui::Text("Keys: %d", m_keys);
		ImGui::TreePop();
	}
}

dfComponentActor::~dfComponentActor()
{
}
