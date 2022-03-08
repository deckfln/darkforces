#include "dfPDA.h"

#include <GLFW/glfw3.h>

#include "../dfConfig.h"

// prepare the dedicated controler for the PDA
static const std::vector<GameEngine::Component::ControllerUI::KeyInfo> g_keys = {
	{GLFW_KEY_TAB, GameEngine::Component::ControllerUI::KeyInfo::Msg::onPressDown},
	{GLFW_KEY_ESCAPE, GameEngine::Component::ControllerUI::KeyInfo::Msg::onPress},
	{GLFW_KEY_SPACE, GameEngine::Component::ControllerUI::KeyInfo::Msg::onPressDown},
	{GLFW_KEY_F1, GameEngine::Component::ControllerUI::KeyInfo::Msg::onPressDown}	// PDA
};


DarkForces::Prefab::PDA::PDA(void):
	gaEntity(DarkForces::ClassID::_PDA, "pda"),
	m_controler(g_keys)
{
	m_physical = false;
	m_gravity = false;
	m_hasCollider = false;

	//m_controler.set(keys);

	// record the components
	addComponent(&m_controler);
	addComponent(&m_pda);

	// and prepare the hud
	m_ui.add(m_pda.hud());
}