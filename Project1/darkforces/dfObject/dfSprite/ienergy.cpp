#include "ienergy.h"

DarkForces::IEnergy::IEnergy(const glm::vec3& p, float ambient, uint32_t content):
	DarkForces::Sprite::WAX("IENERGY.FME", p, ambient)
{
	m_logic.setValue(content);
	hasCollider(true);
}