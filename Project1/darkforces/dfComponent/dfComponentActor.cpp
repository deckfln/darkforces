#include "dfComponentActor.h"

#include <imgui.h>

#include "../../config.h"

#include "../../gaEngine/World.h"

#include "dfWeapon.h"
#include "../weapons.h"
#include "../dfObject/dfSpriteAnimated.h"
#include "../dfObject.h"
#include "../dfBullet.h"
#include "../../gaEngine/gaActor.h"

#include "../flightRecorder/frCompActor.h"

static std::map<std::string, struct DarkForces::ActorClass> m_actors = {
	{ "player", { 1000, 100, glm::vec2(-0.5, 0.5)}},
	{ "COMMANDO.WAX", { 100, 100, glm::vec2(0.2, 0.80)}},
	{ "INTDROID.WAX", { 100, 100, glm::vec2(0.5, 0.75)}},
	{ "OFFCFIN.WAX", { 100, 100, glm::vec2(0.5, 0.85)}},
	{ "STORMFIN.WAX", { 100, 100, glm::vec2(0.5, 0.75)}},
};

DarkForces::Component::Actor::Actor(void) :
	GameEngine::Component::Actor()
{
}

/**
 * extract data from the class
 */
void DarkForces::Component::Actor::setDataFromClass(void)
{
	if (m_actors.count(m_class) > 0) {
		m_life = m_actors[m_class].life;
		m_shield = m_actors[m_class].shield;

		// move the starting position of the bullet around the weapon position (based on the class of the actor)
		DarkForces::Component::Weapon* weapon = dynamic_cast<DarkForces::Component::Weapon*>(m_entity->findComponent(DF_COMPONENT_WEAPON));
		if (weapon) {
			weapon->setActorPosition(m_actors[m_class].weapon);
		}
	}
	else {
		gaDebugLog(0, "DarkForces::Component::Actor::setDataFromClass", m_class + " unknown");
	}
}

DarkForces::Component::Actor::Actor(const std::string& xclass):
	GameEngine::Component::Actor(),
	m_class(xclass)
{
	setDataFromClass();
}

/**
 * Increase or decrease the shield and update the HUD
 */
void DarkForces::Component::Actor::addShield(int32_t value)
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
void DarkForces::Component::Actor::addEnergy(int32_t value)
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
void DarkForces::Component::Actor::hitBullet(int32_t value)
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
 * return data on the class of the actor
 */
const DarkForces::ActorClass* DarkForces::Component::Actor::getActorClass(void)
{
	if (m_actors.count(m_class) > 0) {
		return &m_actors[m_class];
	}
	else {
		gaDebugLog(0, "DarkForces::Component::Actor::setDataFromClass", m_class + " unknown");
		return nullptr;
	}
}

/**
 * Deal with messages
 */
void DarkForces::Component::Actor::dispatchMessage(gaMessage* message)
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

	GameEngine::Component::Actor::dispatchMessage(message);
}

/**
 * kill the actor
 */
void DarkForces::Component::Actor::die(void)
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

	gaDebugLog(1, "ActorClass::die", "remove " + m_entity->name() + " the entity from the world");
}

/**
 * size of the component
 */
inline uint32_t DarkForces::Component::Actor::recordSize(void)
{
	return sizeof(flightRecorder::DarkForces::CompActor);
}

/**
 * save the component state in a record
 */
uint32_t DarkForces::Component::Actor::recordState(void* r)
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
uint32_t DarkForces::Component::Actor::loadState(void* r)
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
void DarkForces::Component::Actor::debugGUIinline(void)
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
	GameEngine::Component::Actor::debugGUIinline();
}