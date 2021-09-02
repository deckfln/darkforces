#include "fwScene.h"

#include <iterator> 
#include <map>
#include <list>
#include <string>
#include <imgui.h>

#include "../glEngine/glColorMap.h"

#include "fwCamera.h"
#include "materials/fwOutlineMaterial.h"

#include "fwLight.h"
#include "fwHUD.h"
#include "fwHUDelement.h"

static const char* g_className = "fwScene";

fwScene::fwScene():
	fwObject3D()
{
	m_className = g_className;
}

fwScene& fwScene::setOutline(glm::vec3* _color)
{
	outlinecolor = _color;
	return *this;
}

fwScene &fwScene::addLight(fwLight *light)
{
	addChild(light);

	m_lights.push_back(light);
	return *this;
}

/**
 * Add a hud element on the scene, create the HUD the first time
 */
fwScene& fwScene::hud(fwHUDelement* element)
{
	if (m_hud == nullptr) {
		m_hud = new fwHUD();
	}

	m_hud->add(element);

	return *this;
}

/**
 * Display the scene content on the debugger
 */
void fwScene::debugGUI(void)
{
	ImGui::Begin("Explorer");
	debugGUItree(m_inspector);
	ImGui::End();

	// display entities monitored
	ImGui::Begin("Inspector");
	for (auto& watch : m_inspector) {
		if (watch.second && watch.first != this) {
			watch.first->debugGUI();
		}
	}
	ImGui::End();
}

fwScene::~fwScene()
{
	if (m_hud != nullptr) {
		delete m_hud;
	}
}
