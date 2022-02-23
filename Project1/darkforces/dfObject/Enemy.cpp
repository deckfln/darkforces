#include "Enemy.h"

#include <map>

#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

#include "../dfModel/dfWAX.h"
#include "../dfVOC.h"
#include "../dfSounds.h"
#include "../gaItem/dfItem/dfWeapon.h"

static const char* g_className = "dfEnemy";

static DarkForces::Item g_RedKey("darkforce:redkey", dfLogic::RED_KEY);

/**
 * when dying, drop all objects from the inventory
 */
void DarkForces::Enemy::onDie(gaMessage* message)
{
	glm::vec3 p = position();

	// ask the inventory to drop all the items
	sendMessage(m_name, gaMessage::Action::DROP_ITEM, -1, m_radius, p);
}

/**
 *
 */
DarkForces::Enemy::Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID):
	DarkForces::Object(model, position, ambient, OBJECT_WAX, objectID)
{
	m_className = g_className;
	m_physical = true;
	m_canStep = true;
	m_step = 0.2061f;

	m_radius = (m_modelAABB.m_p1.x - m_modelAABB.m_p.x) / 2.0f;
	m_cylinder.height(m_modelAABB.height());
	m_cylinder.radius(m_radius);
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);

	addComponent(&m_aiPerception);
	m_aiPerception.audio();
	m_aiPerception.hearSound(DarkForces::Component::Actor::Sound::FIRE);	// enemy react when they hear a bullet hitting a wall

	m_aiPerception.view();
	m_aiPerception.distance(m_radius * 30.0f);
	m_aiPerception.viewEntity("player");		// enemy only reacts if they see the player

	addComponent(&m_actor);
	addComponent(&m_sound);
	addComponent(&m_ai);
	addComponent(&m_weapon);
	addComponent(&m_inventory);

	m_actor.setClass(model->name());

	m_sound.addSound(DarkForces::Component::Actor::Sound::DIE, DarkForces::loadSound(DarkForces::Sounds::STORM_COMMANDO_OFFICER_DIE)->sound());
	m_sound.addSound(DarkForces::Component::Actor::Sound::HURT, DarkForces::loadSound(DarkForces::Sounds::STORM_COMMANDO_OFFICER_HIT_LASER)->sound());

	DarkForces::Component::SpriteAnimated* sprite = new DarkForces::Component::SpriteAnimated(model, ambient);
	addComponent(sprite);
}

/**
 * extend the object at the end of the load
 */
void DarkForces::Enemy::extend(void)
{
	// set sounds
	std::map<std::string, std::string> includes;

	if (m_logics & dfLogic::RED_KEY) {
		m_inventory.add(&g_RedKey);
	}

	m_inventory.add(&m_clip);

	if (m_logics & dfLogic::OFFICER) {
		m_currentWeapon.clone(&g_Pistol);
		m_inventory.add(&m_currentWeapon);

		m_currentWeapon.loadClip();
		m_weapon.set(&m_currentWeapon);

		includes["sounds.inc"] = "<sound file = 'RANOFC02.voc' id = '2048' />\
			< sound file = 'RANOFC04.voc' id = '2050' /> \
			<sound file = 'RANOFC05.voc' id = '2051' />\
			<sound file = 'RANOFC06.voc' id = '2052' />";
	}
	else {
		m_currentWeapon.clone(&g_Rifle);
		m_inventory.add(&m_currentWeapon);
		m_currentWeapon.loadClip();
		m_weapon.set(&m_currentWeapon);

		includes["sounds.inc"] = "<sound file = 'Ransto01.voc' id = '2048' />\
			< sound file = 'Ransto02.voc' id = '2049' /> \
			<sound file = 'Ransto03.voc' id = '2050' />\
			<sound file = 'Ransto04.voc' id = '2051' />\
			<sound file = 'Ransto05.voc' id = '2052' />\
			<sound file = 'Ransto06.voc' id = '2053' />\
			<sound file = 'Ransto07.voc' id = '2054' />\
			<sound file = 'Ransto08.voc' id = '2055' />";
	}

	m_ai.parse("file:DarkForces/AI/Enemy.xml", includes);

	sendMessage(gaMessage::Action::MOVE);
}

/**
 *
 */
void DarkForces::Enemy::setLevel(dfLevel* level)
{
	m_actor.bind(level);
}

/**
 *
 */
void DarkForces::Enemy::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::DYING:
		onDie(message);
	}

	DarkForces::Object::dispatchMessage(message);
}

DarkForces::Enemy::~Enemy()
{
}
