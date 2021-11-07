#include "dfRifle.h"

DarkForces::Sprite::Rifle::Rifle(const glm::vec3& p, float ambient, uint32_t content):
	DarkForces::Sprite::WAX("IST-GUNI.FME", p, ambient)
{
	m_logic.setValue(content);
	hasCollider(true);
}