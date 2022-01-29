#include "dfEnergy.h"

DarkForces::Prefab::Energy::Energy(const glm::vec3& p, uint32_t value):
	DarkForces::Sprite::WAX("IENERGY.FME", p)
{
	m_logic.setValue(value);
	hasCollider(true);
	logic(dfLogic::ITEM_ENERGY);
}
