#include "dfRedKey.h"

DarkForces::Prefab::RedKey::RedKey(const glm::vec3& p):
	DarkForces::Sprite::FME("IKEYR.FME", p, 1.0f)
{
	hasCollider(true);
	m_logic.logic(dfLogic::RED_KEY);
}