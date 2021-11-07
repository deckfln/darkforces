#include "dfRifle.h"

DarkForces::Sprite::Rifle::Rifle(const glm::vec3& p, float ambient, uint32_t content):
	DarkForces::Object("IST-GUNI.FME", p)
{
	m_sprite = new DarkForces::Component::Sprite("IST-GUNI.FME", ambient);
	addComponent(m_sprite);

	m_logic.setValue(content);
	hasCollider(true);
}