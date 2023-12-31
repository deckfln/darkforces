#include "dfCActor.h"

#include <imgui.h>

#include "../../config.h"

#include "../../gaEngine/World.h"

#include "../dfComponent.h"

#include "dfCWeapon.h"
#include "../weapons.h"
#include "../dfObject.h"
#include "../dfBullet.h"
#include "../../gaEngine/gaActor.h"

#include "../flightRecorder/frCompActor.h"

static std::map<std::string, struct DarkForces::ActorClass> m_actors = {
	{ "player", { 1000, 100, glm::vec2(-0.5, 0.5)}},
	{ "COMMANDO.WAX", { 10, 100, glm::vec2(0.2, 0.80)}},
	{ "INTDROID.WAX", { 10, 100, glm::vec2(0.5, 0.75)}},
	{ "OFFCFIN.WAX", { 10, 100, glm::vec2(0.5, 0.85)}},
	{ "STORMFIN.WAX", { 10, 100, glm::vec2(0.5, 0.75)}},
	{ "MOUSEBOT.3DO", { 1, 0, glm::vec2(0)}},
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

		// inform the entity of the new shield value
		m_entity->sendMessage(DarkForces::Message::SHIELD, m_shield);
		m_entity->sendMessage(DarkForces::Message::LIFE, m_life);
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
	if (m_dying) {
		return;
	}

	m_shield += value;
	if (m_shield > m_maxShield) {
		m_shield = m_maxShield;
	}
	else if (m_shield < 0) {
		m_shield = 0;
	}

	// inform the entity of the new shield value
	m_entity->sendMessage(DarkForces::Message::SHIELD, m_shield);
}

/**
 * hit by a bullet, reduce shield and life
 */
void DarkForces::Component::Actor::onHitBullet(int32_t value)
{
	if (m_dying) {
		return;
	}

	if (m_shield > 0) {
		m_shield -= value;

		// inform the entity of the new shield value
		m_entity->sendMessage(DarkForces::Message::SHIELD, m_shield);
	}
	else {
		m_life -= value;
		if (m_life < 0) {
			die();
			return;
		}
		m_entity->sendMessage(DarkForces::Message::LIFE, m_life);
	}

	// play a sound if there is one
	m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Component::Actor::Sound::HURT);
}

/**
 * when the dying animation starts
 * move the actor back compared to the player
 */
void DarkForces::Component::Actor::onDying(gaMessage* message)
{
	m_dying = true;

	// activate the entity last animation
	m_entity->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_DIE_FROM_SHOT);

	// play a sound if there is one
	m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Component::Actor::Sound::DIE);
}

/**
 * when animations stats
 */
void DarkForces::Component::Actor::onAnimStart(gaMessage * message)
{
	if (!m_dying) {
		return;
	}

	gaEntity* player = g_gaWorld.getEntity("player");
	glm::vec3 p = m_entity->position();

	// fallback by the actor radius
	m_dyingDirection = glm::normalize(p - player->position()) * 2.0f * m_entity->radius();

	float frames = (float)(*(uint32_t*)message->m_extra);
	m_dyingDirection /= frames;
}

/**
 * when animations are running
 */
void DarkForces::Component::Actor::onAnimNextFrame(gaMessage* message)
{
	if (!m_dying) {
		return;
	}

	// only do something if the player is dying
	glm::vec3 p = m_entity->position();
	GameEngine::Transform* transform = m_entity->pTransform();
	transform->m_position = p + m_dyingDirection;

	// the actor can fall off an edge when dying
	m_entity->sendMessage(gaMessage::Action::WANT_TO_MOVE, gaMessage::Flag::WANT_TO_MOVE_FALL, transform);
}

/**
 * when the dying animation stops
 */
void DarkForces::Component::Actor::onDead(gaMessage* message)
{
	// object can now be traversed
	m_entity->hasCollider(false);
	m_entity->physical(false);

	m_entity->discardMessages();

	gaDebugLog(1, "DarkForces::Component::Actor::die", "kill the entity " + m_entity->name() + " and block from the world");
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
 * start the kill actor sequence
 */
void DarkForces::Component::Actor::die(void)
{
	// cancel any message the entity was expecting
	g_gaWorld.deleteMessages(m_entity);

	// let everyone this is the last stage
	m_entity->sendMessage(DarkForces::Message::DYING, 0);
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

	case gaMessage::Action::BULLET_HIT:
		onHitBullet(message->m_value);
		break;

	case gaMessage::MOVE:
		if (m_level == nullptr) {
			break;
		}

		// identity the DF sector and trigger enter/leave if changing
		if (message->m_extra == nullptr) {
			dfSector* current = m_level->findSector(m_entity->position());
			if (current != m_currentSector) {
				if (m_currentSector != nullptr) {
					m_entity->sendMessage(m_currentSector->name(), DarkForces::Message::EVENT, DarkForces::MessageEvent::LEAVE_SECTOR);
					m_currentSector->sendMessage(m_entity->name(), DarkForces::Message::EVENT, DarkForces::MessageEvent::LEAVE_SECTOR);

					if (current != nullptr) {
						m_entity->sendMessage(current->name(), DarkForces::Message::EVENT, DarkForces::MessageEvent::ENTER_SECTOR);
						current->sendMessage(m_entity->name(), DarkForces::Message::EVENT, DarkForces::MessageEvent::ENTER_SECTOR);
					}
				}
				m_currentSector = current;
			}
		}
		break;

	case DarkForces::Message::DYING:
		onDying(message);
		break;

	case DarkForces::Message::ANIM_START:
		onAnimStart(message);
		break;

	case DarkForces::Message::ANIM_NEXT_FRAME:
		onAnimNextFrame(message);
		break;

	case DarkForces::Message::DEAD:
		onDead(message);
		break;
	}

	GameEngine::Component::Actor::dispatchMessage(message);
}

//*****************************************************

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
	GameEngine::Component::Actor::recordState(&record->actor);
	record->actor.size = sizeof(flightRecorder::DarkForces::CompActor);
	record->id = m_id;

	record->shield = m_shield;
	record->maxShield = m_maxShield;
	record->battery = m_battery;
	record->life = m_life;
	record->keys = static_cast<uint32_t>(m_keys);

	return record->actor.size;
}

/**
 * reload a component state from a record
 */
uint32_t DarkForces::Component::Actor::loadState(void* r)
{
	flightRecorder::DarkForces::CompActor* record = (flightRecorder::DarkForces::CompActor*)r;
	GameEngine::Component::Actor::loadState(&record->actor);

	m_shield = record->shield;
	m_maxShield = record->maxShield;
	m_battery = record->battery;
	m_life = record->life;
	m_keys = static_cast<DarkForces::Keys>(record->keys);

	return record->actor.size;
}

//*****************************************************

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
		ImGui::Text("Battery: %d", m_battery);
		ImGui::Text("Life: %d", m_life);
		ImGui::Text("Keys: %d", m_keys);
		ImGui::TreePop();
	}
	GameEngine::Component::Actor::debugGUIinline();
}