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
 * Add a new flatUI on top of the previous one
 */

void fwScene::hud(fwHUD* ui)
{
	m_uis.push_back(ui);
}

/**
 * ig there are UI's
 */
bool fwScene::hasUI(void)
{
	return m_uis.size() > 0;
}

/**
 * Draw the UI's
 */
void fwScene::drawUI(void)
{
	for (auto ui : m_uis) {
		ui->draw();
	}
}

#ifdef _DEBUG
/**
 * Display the scene content on the debugger
 */
void fwScene::debugGUI(bool debug)
{
	ImGui::Begin("Explorer");
	if (debug) {
		debugGUItree(m_inspector);
	}
	ImGui::End();

	// display entities monitored
	ImGui::Begin("Inspector");
	if (debug) {
		for (auto& watch : m_inspector) {
			if (watch.second && watch.first != this) {
				watch.first->debugGUI();
			}
		}
	}
	ImGui::End();
}
#endif

/**
 * add a 2D element
 */
void fwScene::addMesh2D(Framework::Mesh2D* mesh)
{
	m_meshes2D.push_back(mesh);
}
