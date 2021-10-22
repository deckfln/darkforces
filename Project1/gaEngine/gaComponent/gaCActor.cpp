#include "gaCActor.h"

#include <imgui.h>

#ifdef _DEBUG
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

#include "../../framework/fwMaterialBasic.h"
#include "../../framework/geometries/fwGeometryCylinder.h"

static glm::vec4 g_red(1.0, 1.0, 0.0, 1.0);
static fwMaterialBasic g_basic(&g_red);
static fwGeometryCylinder* g_view = nullptr;

/**
 * add/remove a vector mesh
 */
void GameEngine::Component::Actor::directionVector(void)
{
	if (m_debug) {
		// add a point of view vector
		if (g_view == nullptr) {
			g_basic.makeStatic();
			g_view = new fwGeometryCylinder(0.01f, 0.15f, 4, 1);
			g_view->makeStatic();
		}
		if (m_view == nullptr) {
			m_view = new fwMesh(g_view, &g_basic);
		}

		float a = atan2(m_direction.x, m_direction.z);// +M_PI / 2.0f;
		glm::vec3 xr(0, a, 0);
		m_view->rotate(xr);
		m_view->translate(m_entity->get_position() + glm::vec3(0.0f, 0.5f, 0.0f));

		g_gaWorld.add2scene(m_view);
	}
	else {
		g_gaWorld.remove2scene(m_view);
	}
}
#endif

GameEngine::Component::Actor::Actor(void) :
	gaComponent(gaComponent::Actor)
{
}

void GameEngine::Component::Actor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::LOOK_AT:
		if (message->m_extra == nullptr) {
			m_direction = glm::normalize(message->m_v3value);
		}
	}
}

void GameEngine::Component::Actor::debugGUIinline(void)
{
	if (ImGui::TreeNode("Actor")) {
#ifdef _DEBUG
		// display/hide the direction vector of the sprite
		bool b = m_debug;
		ImGui::Checkbox("Debug direction", &m_debug);
		if (b != m_debug) {
			directionVector();
		}
#endif

		ImGui::Text("lookAt:%.2f %.2f %.2f", m_direction.x, m_direction.y, m_direction.z);
		ImGui::TreePop();
	}
}
