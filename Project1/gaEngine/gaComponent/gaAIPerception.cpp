#include "gaAIPerception.h"

#include <imgui.h>

#include "../World.h"
#include "../gaActor.h"

GameEngine::Component::AIPerception::AIPerception(bool view, bool audio, float distance, float angle) :
	m_view(view),
	m_audio(audio),
	m_distance(distance),
	m_angle(angle),
	gaComponent(gaComponent::AIPerception)
{
	if (m_entity) {
		if (m_view) {
			g_gaWorld.registerViewEvents(this->m_entity);
		}
		if (m_audio) {
			g_gaWorld.registerHearEvents(this->m_entity);
		}
	}
}

void GameEngine::Component::AIPerception::debugGUIinline(void)
{
	if (ImGui::TreeNode("AIPerception")) {
		if (m_view) {
			ImGui::Text("View");
			ImGui::Text(" distance:%f", m_distance);
			ImGui::Text(" angle:%f", m_angle);
		}

		if (m_audio) {
			ImGui::Text("Hear");
		}
		ImGui::TreePop();
	}
}

void GameEngine::Component::AIPerception::registerEvents(void)
{
	if (m_view) {
		g_gaWorld.registerViewEvents(this->m_entity);
	}
	if (m_audio) {
		g_gaWorld.registerHearEvents(this->m_entity);
	}
}

GameEngine::Component::AIPerception::~AIPerception()
{
	g_gaWorld.deRegisterViewEvents(this->m_entity);
	g_gaWorld.deRegisterHearEvents(this->m_entity);
}

