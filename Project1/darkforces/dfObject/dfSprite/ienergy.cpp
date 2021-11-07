#include "ienergy.h"

DarkForces::IEnergy::IEnergy(const glm::vec3& p, float ambient, uint32_t content):
	DarkForces::Object("IENERGY.FME", p)
{
	m_sprite = new DarkForces::Component::Sprite("IENERGY.FME", ambient);
	addComponent(m_sprite);

	m_logic.setValue(content);
	hasCollider(true);
}