#include "dfCPDA.h"

#include <imgui.h>

#include "../../gaEngine/gaMessage.h"
#include "../dfComponent.h"
#include "../dfMessage.h"

DarkForces::ANIM* DarkForces::Component::PDA::m_pda = nullptr;
DarkForces::ANIM* DarkForces::Component::PDA::m_guns = nullptr;
DarkForces::ANIM* DarkForces::Component::PDA::m_items = nullptr;
DarkForces::DELT* DarkForces::Component::PDA::m_pda_background = nullptr;
DarkForces::HUDelement::PDA* DarkForces::Component::PDA::m_hud = nullptr;
Framework::TextureAtlas* DarkForces::Component::PDA::m_items_textures = nullptr;

/**
 * initialize the PDA
 */
void DarkForces::Component::PDA::onWorldInsert(gaMessage*)
{
	if (m_pda == nullptr) {
		// preload the PDA background
		m_pda = DarkForces::FileLFD::loadAnim("pda", "MENU");

		m_items_textures = new Framework::TextureAtlas();

		// load all guns into a texturearray
		m_guns = DarkForces::FileLFD::loadAnim("guns", "DFBRIEF");
		size_t nbItems = m_guns->size();
		for (size_t i = 0; i < nbItems; i++) {
			m_items_textures->add(m_guns->texture(i)->texture());
		}

		// load all items into a texturearray
		m_items = DarkForces::FileLFD::loadAnim("items", "DFBRIEF");
		nbItems = m_items->size();
		for (size_t i = 0; i < nbItems; i++) {
			m_items_textures->add(m_items->texture(i)->texture());
		}

		m_items_textures->generate();

		m_pda_background = m_pda->texture(0);
	}
}

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
		case gaMessage::WORLD_INSERT:
			onWorldInsert(message);
			break;
		case DarkForces::Message::PDA:
			onShowPDA(message);
			break;
	}
}

/**
 * initiliaze and return and hud element
 */
fwHUDelement* DarkForces::Component::PDA::hud(void)
{
	if (m_hud == nullptr) {
		m_hud = new DarkForces::HUDelement::PDA("pda", fwHUDelement::Position::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 1.0f, 1.0f, m_pda_background->texture());
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