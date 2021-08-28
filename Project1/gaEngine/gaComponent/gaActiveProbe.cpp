#include "gaActiveProbe.h"

#include "../../config.h"
#include "../gaEntity.h"
#include "../World.h"
#include "../../gaEngine/gaBoundingBoxes.h"

GameEngine::Component::ActiveProbe::ActiveProbe(void):
	gaComponent(gaComponent::ActiveProbe),
	m_worldMatrix(glm::mat4x4(1)),
	m_inverseWorldMatrix(glm::inverse(m_worldMatrix))
{
	m_collider.set(&m_segment, &m_worldMatrix, &m_inverseWorldMatrix);
	g_gaBoundingBoxes.add(&m_worldAABB);
}

void GameEngine::Component::ActiveProbe::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::MOVE_AT:
		if (message->m_extra != nullptr) {
			m_segment.m_start = *(glm::vec3*)message->m_extra;
		}
		else {
			m_segment.m_start = m_entity->position();
		}
		m_segment.m_end = m_segment.m_start + m_direction;
		m_worldAABB.set(m_segment.m_start, m_segment.m_end);
		break;

	case gaMessage::Action::LOOK_AT:
		m_direction = glm::normalize(*(glm::vec3*)message->m_extra)/1.5f;
		m_segment.m_end = m_segment.m_start, m_direction;
		m_worldAABB.set(m_segment.m_start, m_segment.m_end);
		break;

	case gaMessage::KEY:
		if (message->m_value == 32) {
			gaEntity* collidedEntity=nullptr;

			// check if entities with component TRIGGER intersect with the segment m_start-m_direction
			std::vector<gaEntity *> collisions;

			g_gaWorld.intersectWithEntity(DF_COMPONENT_TRIGGER, m_segment, collisions);
			for (auto entity: collisions) {
				// activate the entity intersecting with the probe
				m_entity->sendMessage(entity->name(), gaMessage::Action::ACTIVATE);
			}
		}
	}
}

void GameEngine::Component::ActiveProbe::debugGUIinline(void)
{
}
