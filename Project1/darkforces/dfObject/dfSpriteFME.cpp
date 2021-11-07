#include "dfSpriteFME.h"

DarkForces::Sprite::FME::FME(const std::string& model, const glm::vec3& p, float ambient):
	DarkForces::Object(model, p)
{
	m_sprite.set(model, ambient);
	addComponent(&m_sprite);
}