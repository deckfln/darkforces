#include "ienergy.h"

DarkForces::IEnergy::IEnergy(const glm::vec3& p, float ambient, uint32_t content):
	dfSprite("IENERGY.FME", p, 1.0f, OBJECT_FME)
{
	m_componentLogic.setValue(content);
	hasCollider(true);
}