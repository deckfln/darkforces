#include "Enemy.h"

#include "../../../dfModel/dfWAX.h"

static const char* g_className = "dfEnemy";

DarkForces::Enemy::Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID):
	dfSpriteAnimated(model, position, ambient, objectID)
{
	m_className = g_className;
	m_physical = true;
	m_canStep = true;
	m_step = 0.2061f;

	m_radius = (m_modelAABB.m_p1.x - m_modelAABB.m_p.x) / 2.0f;
	m_cylinder.height(m_modelAABB.height());
	m_cylinder.radius(m_radius);
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);

	addComponent(&m_actor);

	addComponent(&m_aiPerception);
	m_aiPerception.distance(m_radius * 30.0f);
	m_aiPerception.registerEvents();

	addComponent(&m_weapon);

	if (model->name() == "OFFCFIN.WAX") {
		m_weapon.set(DarkForces::Component::Weapon::Kind::Pistol);
	}
	else {
		m_weapon.set(DarkForces::Component::Weapon::Kind::Rifle);
	}
}

DarkForces::Enemy::~Enemy()
{
}
