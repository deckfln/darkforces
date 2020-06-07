#include "fwScene.h"
#include <iterator> 
#include <map>
#include <list>
#include <string>

#include "fwLight.h"
#include "fwHUD.h"
#include "fwHUDelement.h"

fwScene::fwScene()
{
}

fwScene& fwScene::setOutline(glm::vec3* _color)
{
	outlinecolor = _color;
	return *this;
}

fwScene &fwScene::addLight(fwLight *light)
{
	addChild(light);

	lights.push_back(light);
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

fwScene::~fwScene()
{
	if (m_hud != nullptr) {
		delete m_hud;
	}
}
