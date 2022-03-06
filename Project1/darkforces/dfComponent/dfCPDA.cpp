#include "dfCPDA.h"

#include <map>
#include <imgui.h>

#include "../../gaEngine/gaMessage.h"
#include "../../gaEngine/gaItem.h"
#include "../dfComponent.h"
#include "../dfMessage.h"

DarkForces::ANIM* DarkForces::Component::PDA::m_pda = nullptr;
DarkForces::ANIM* DarkForces::Component::PDA::m_guns = nullptr;
DarkForces::ANIM* DarkForces::Component::PDA::m_items = nullptr;
DarkForces::DELT* DarkForces::Component::PDA::m_pda_background = nullptr;
DarkForces::HUDelement::PDA* DarkForces::Component::PDA::m_hud = nullptr;
Framework::TextureAtlas* DarkForces::Component::PDA::m_items_textures = nullptr;

static std::map<std::string, uint32_t> g_dfItems = {
	{"Pistol", 0},
	{"Rifle", 1}
};

/**
 * display the PDA
 */
void DarkForces::Component::PDA::onShowPDA(gaMessage*)
{
	if (m_hud->visible()) {
		m_hud->visible(false);
	}
	else {
		m_hud->visible(true);
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
		m_hud->activateGun(g_dfItems[item->name()]);
	}
}

/**
 * create
 */
DarkForces::Component::PDA::PDA(void):
	gaComponent(DF_COMPONENT_PDA)
{
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

		case gaMessage::ADD_ITEM:
			onAddItem(message);
			break;

	}
}

/**
 * initiliaze and return and hud element
 */
fwHUDelement* DarkForces::Component::PDA::hud(void)
{
	if (m_hud == nullptr) {
		m_hud = new DarkForces::HUDelement::PDA("pda", fwHUDelement::Position::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 1.0f, 1.0f);
		m_hud->visible(false);
	}
	return m_hud;
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
		delete m_hud;
	}
}