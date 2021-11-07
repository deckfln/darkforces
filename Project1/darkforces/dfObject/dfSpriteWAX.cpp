#include "dfSpriteWAX.h"

DarkForces::Sprite::WAX::WAX(const std::string& model, const glm::vec3& p, float ambient):
	DarkForces::Object(model, p)
{
	m_sprite.set(model, ambient);
	addComponent(&m_sprite);
}