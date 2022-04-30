#include "gaCImposter.h"

#include "../gaEntity.h"

/**
 * creation
 */
GameEngine::Component::Imposter::Imposter(void):
	gaComponent(gaComponent::Imposter)
{
}

const std::vector<gaEntity*>& GameEngine::Component::Imposter::entitiesOnTop(void)
{
	static std::vector<gaEntity*> keys;

	keys.clear();

	for (auto& entry : m_entitiesAbove) {
		keys.push_back(entry.first);
	}

	return keys;
}

/**
 * remove one entity sitting on top of that entity
 */
void GameEngine::Component::Imposter::removeEntityAbove(gaEntity* entity)
{
	m_entitiesAbove.erase(entity);
}

/**
 * unlink all entities this entity is sitting on
 */
void GameEngine::Component::Imposter::clearEntitiesBelow(void)
{
	GameEngine::Component::Imposter* imposter = nullptr;
	
	for (auto& entry : m_entitiesBelow) {
		// unlink the entity below
		imposter = dynamic_cast<GameEngine::Component::Imposter*>(entry.first->findComponent(gaComponent::Imposter));
		if (imposter) {
			imposter->removeEntityAbove(this->m_entity);
		}
	} 

	m_entitiesBelow.clear();
}
