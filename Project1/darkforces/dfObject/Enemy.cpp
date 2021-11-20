#include "Enemy.h"

#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

#include "../dfWeapon.h"
#include "../dfModel/dfWAX.h"
#include "../dfVOC.h"
#include "../dfSounds.h"

static const char* g_className = "dfEnemy";

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
	m_aiPerception.hearSound(DarkForces::loadSound(DarkForces::Sounds::WALL_HIT_LASER)->sound());	// enemy react when they hear a bullet hitting a wall
	m_aiPerception.hearSound(DarkForces::loadSound(DarkForces::Sounds::STORM_COMMANDO_OFFICER_HIT_LASER)->sound());	// enemy react when they hear a bullet hitting an other enemy

	m_aiPerception.view();
	m_aiPerception.distance(m_radius * 30.0f);
	m_aiPerception.viewEntity("player");		// enemy only reacts if they see the player

	addComponent(&m_actor);
	addComponent(&m_sound);
	addComponent(&m_ai);
	addComponent(&m_weapon);

	m_actor.setClass(model->name());
	if (model->name() == "OFFCFIN.WAX") {
		m_weapon.set(DarkForces::Weapon::Kind::Pistol);

		// load sounds
		m_ai.addSound("RANOFC02.voc", 2048);
		m_ai.addSound("RANOFC04.voc", 2050);
		m_ai.addSound("RANOFC05.voc", 2051);
		m_ai.addSound("RANOFC06.voc", 2052);
	}
	else {
		m_weapon.set(DarkForces::Weapon::Kind::Rifle);

		// load sounds
		m_ai.addSound("Ransto01.voc", 2048);
		m_ai.addSound("Ransto02.voc", 2049);
		m_ai.addSound("Ransto03.voc", 2050);
		m_ai.addSound("Ransto04.voc", 2051);
		m_ai.addSound("Ransto05.voc", 2052);
		m_ai.addSound("Ransto06.voc", 2053);
		m_ai.addSound("Ransto07.voc", 2054);
		m_ai.addSound("Ransto08.voc", 2055);
	}
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
