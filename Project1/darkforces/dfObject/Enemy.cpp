#include "Enemy.h"

#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

#include "../dfWeapon.h"
#include "../dfModel/dfWAX.h"
#include "../dfVOC.h"

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
	m_aiPerception.distance(m_radius * 30.0f);
	m_aiPerception.registerEvents();

	addComponent(&m_actor);
	addComponent(&m_sound);
	addComponent(&m_ai);
	addComponent(&m_weapon);

	m_actor.setClass(model->name());
	if (model->name() == "OFFCFIN.WAX") {
		m_weapon.set(DarkForces::Weapon::Kind::Pistol);
	}
	else {
		m_weapon.set(DarkForces::Weapon::Kind::Rifle);
	}
	m_weapon.addEnergy(200);

	m_sound.addSound(DarkForces::Component::Actor::Sound::DIE, loadVOC("ST-DIE-1.voc")->sound());
	m_sound.addSound(DarkForces::Component::Actor::Sound::HURT, loadVOC("ST-HRT-1.voc")->sound());

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
