#include "gaPView.h"

#include "../gaEntity.h"
#include "../gaComponent/gaAIPerception.h"
#include "../gaComponent/gaCActor.h"
#include "../World.h"

GameEngine::Plugins::View g_gaViewEngine;

/**
 *
 */
GameEngine::Plugins::View::View(void) :
	GameEngine::Plugin("View")
{
}

/**
 * execute the plugin before processing the message queue
 */
void GameEngine::Plugins::View::beforeProcessing(void)
{
	gaEntity* viewed;
	gaEntity* viewer;
	Component::AIPerception* perception;
	Component::Actor* actor;
	bool seen;

	// check visual perceptions
	for (auto& pair : m_views) {
		viewer = pair.second;
		perception = dynamic_cast<Component::AIPerception*>(viewer->findComponent(gaComponent::AIPerception));

		// for every entity the viewer wants to see
		const std::vector<std::string>& viewedEntities = perception->viewedEntities();

		for (auto& s : viewedEntities) {
			viewed = g_gaWorld.getEntity(s);

			seen = false;
			// the player is in the entity distance perception
			if (viewed->distanceTo(viewer) < perception->distance()) {
				actor = dynamic_cast<Component::Actor*>(viewer->findComponent(gaComponent::Actor));

				glm::vec3 d = viewed->position() - viewer->position();
				glm::vec3 v = actor->direction();

				//printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", player->position().x, player->position().z, entity->position().x, entity->position().z, actor->direction().x, actor->direction().z);
				// the player is in front of the entity
				if (glm::dot(d, v) > 0) {

					//TODO: the player is in the entity cone of vision

					// the player is in the line of sight of the entity
					if (g_gaWorld.lineOfSight(viewed, viewer)) {
						seen = true;
					}
				}
			}

			if (seen) {
				viewed->sendMessage(viewer->name(), gaMessage::Action::VIEW, viewed->position(), nullptr);
				perception->view(viewed->entityID(), true);
			}
			else if (perception->view(viewed->entityID())) {
				viewed->sendMessage(viewer->name(), gaMessage::Action::NOT_VIEW);
				perception->view(viewed->entityID(), false);
			}
		}
	}
}

/**
 *  (de)register entities for audio/visual perceptions
 */
void GameEngine::Plugins::View::registerViewEvents(gaEntity* entity)
{
	uint32_t id = entity->entityID();
	if (m_views.count(id) == 0) {
		m_views[id] = entity;
	}
}

void GameEngine::Plugins::View::deregisterViewEvents(gaEntity* entity)
{
	uint32_t id = entity->entityID();
	if (m_views.count(id) != 0) {
		m_views.erase(id);
	}
}