#include "Enemy.h"

#include <map>

#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

#include "../dfWeapon.h"
#include "../dfModel/dfWAX.h"
#include "../dfVOC.h"
#include "../dfSounds.h"

static const char* g_className = "dfEnemy";

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

	m_actor.setClass(model->name());

	// set sounds
	std::string sounds;
	std::map<std::string, std::string> includes;

	if (model->name() == "OFFCFIN.WAX") {
		m_weapon.set(DarkForces::Weapon::Kind::Pistol);

		includes["sounds.inc"] = "<sound file = 'RANOFC02.voc' id = '2048' />\
			< sound file = 'RANOFC04.voc' id = '2050' /> \
			<sound file = 'RANOFC05.voc' id = '2051' />\
			<sound file = 'RANOFC06.voc' id = '2052' />";
	}
	else {
		m_weapon.set(DarkForces::Weapon::Kind::Rifle);

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


	m_weapon.addEnergy(200);

	m_sound.addSound(DarkForces::Component::Actor::Sound::DIE, DarkForces::loadSound(DarkForces::Sounds::STORM_COMMANDO_OFFICER_DIE)->sound());
	m_sound.addSound(DarkForces::Component::Actor::Sound::HURT, DarkForces::loadSound(DarkForces::Sounds::STORM_COMMANDO_OFFICER_HIT_LASER)->sound());

	DarkForces::Component::SpriteAnimated* sprite = new DarkForces::Component::SpriteAnimated(model, ambient);
	addComponent(sprite);
}

void DarkForces::Enemy::setLevel(dfLevel* level)
{
	m_actor.bind(level);
}

DarkForces::Enemy::~Enemy()
{
}
