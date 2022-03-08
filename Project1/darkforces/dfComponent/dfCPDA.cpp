#include "dfCPDA.h"

#include <map>
#include <imgui.h>

#include "../../framework/fwControl.h"
#include "../../gaEngine/gaMessage.h"
#include "../../gaEngine/gaItem.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaApp.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaControlerUI.h"

#include "../dfComponent.h"
#include "../dfMessage.h"

static std::map<std::string, uint32_t> g_dfItems = {
	{"Pistol", 0},
	{"Rifle", 1}
};

/**
 * display the PDA
 */
void DarkForces::Component::PDA::onShowPDA(gaMessage*)
{
	if (m_ui->visible()) {
		m_ui->visible(false);
		GameEngine::World::popState();				// restart the game
		GameEngine::App::popControl();				// restore the correct controler
	}
	else {
		m_ui->visible(true);
		GameEngine::World::pushState();				// suspend the game

		// push the current game controller and replace with the one of the PDA
		GameEngine::Component::ControllerUI* controller = dynamic_cast<GameEngine::Component::ControllerUI *>(m_entity->findComponent(gaComponent::Controller));
		GameEngine::App::pushControl(controller);	// save the game controler and install ours
	}
}

/**
 * addItem on the PDA
 */
void DarkForces::Component::PDA::onAddItem(gaMessage* message)
{
	// activate the item on the PDA
	GameEngine::Item* item = static_cast<GameEngine::Item*>(message->m_extra);
	if (g_dfItems.count(item->name()) > 0) {
		m_ui_guns->activateGun(g_dfItems[item->name()]);
	}
}

/**
 * deal with UI keyboard
 */
void DarkForces::Component::PDA::onKeyDown(gaMessage* message)
{
	switch (message->m_value) {
	case GLFW_KEY_F1:
		onShowPDA(message);		// close the PDA
		break;
	}
}

/**
 * create
 */
DarkForces::Component::PDA::PDA(void):
	gaComponent(DF_COMPONENT_PDA)
{
	m_ui = new fwHUD("DarkForces::PDA");
	m_ui->visible(false);	// hide by default
	m_ui_guns = new DarkForces::HUDelement::PDA("guns", fwHUDelement::Position::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 1.0f, 1.0f);
	m_ui->add(m_ui_guns);
}

/**
 * handle messages
 */
void DarkForces::Component::PDA::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
		case DarkForces::Message::PDA:
			onShowPDA(message);
			break;

		case gaMessage::Action::KEY:
			onKeyDown(message);
			break;

		case gaMessage::ADD_ITEM:
			onAddItem(message);
			break;

	}
}

#ifdef _DEBUG
void DarkForces::Component::PDA::debugGUIinline(void)
{
	if (ImGui::TreeNode("dfCPDA")) {
		ImGui::TreePop();
	}
}
#endif

DarkForces::Component::PDA::~PDA(void)
{
	if (m_pda != nullptr) {
		delete m_pda;
		delete m_guns;
		delete m_items;
		delete m_pda_background;
		delete m_ui;
	}
}