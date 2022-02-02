#include "dfSpriteWAX.h"

DarkForces::Sprite::WAX::WAX(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectid) :
	DarkForces::Object(model, p, objectid)
{
	m_sprite.set(model, ambient);
	addComponent(&m_sprite);
}

DarkForces::Sprite::WAX::WAX(const std::string& model, const glm::vec3& p, float ambient):
	DarkForces::Object(model, p)
{
	m_sprite.set(model, ambient);
	addComponent(&m_sprite);
}