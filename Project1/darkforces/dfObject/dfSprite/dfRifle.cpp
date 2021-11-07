#include "dfRifle.h"

DarkForces::Sprite::Rifle::Rifle(const glm::vec3& p, float ambient, uint32_t content):
	dfSprite("IST-GUNI.FME", p, 1.0f, OBJECT_FME)
{
	m_logic.setValue(content);
	hasCollider(true);
}