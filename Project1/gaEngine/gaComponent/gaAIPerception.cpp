#include "gaAIPerception.h"

#include <imgui.h>

#include "../World.h"
#include "../gaActor.h"

GameEngine::Component::AIPerception::AIPerception(float distance, float angle) :
	m_distance(distance),
	m_angle(angle),
	gaComponent(gaComponent::AIPerception)
{
	if (m_entity) {
		g_gaWorld.registerViewEvents(this->m_entity);
	}
}

void GameEngine::Component::AIPerception::debugGUIinline(void)
{
	if (ImGui::TreeNode("AIPerception")) {
		ImGui::Text("distance:%f", m_distance);
		ImGui::Text("angle:%f", m_angle);
		ImGui::TreePop();
	}
}

void GameEngine::Component::AIPerception::registerEvents(void)
{
	g_gaWorld.registerViewEvents(this->m_entity);
}

GameEngine::Component::AIPerception::~AIPerception()
{
	g_gaWorld.deRegisterViewEvents(this->m_entity);
}

