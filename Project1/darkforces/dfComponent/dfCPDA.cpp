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
#include "../../gaEngine/gaUI.h"

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
	if (m_visible) {
		m_visible = false;
		GameEngine::World::popState();				// restart the game
		GameEngine::App::popControl();				// restore the correct controler
	}
	else {
		m_visible = true;
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
		m_ui_weapons->widget(g_dfItems[item->name()])->visible(true);
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
DarkForces::Component::PDA::PDA(const std::string& name):
	GameEngine::UI(name)
{
	m_visible = false;

	// preload the PDA images
	DarkForces::ANIM* pda = nullptr;
	DarkForces::ANIM* guns = nullptr;
	DarkForces::ANIM* items = nullptr;

	m_textures = new Framework::TextureAtlas();

	pda = DarkForces::FileLFD::loadAnim("pda", "MENU");
	size_t nbItems = pda->size();
	for (size_t i = 0; i < nbItems; i++) {
		m_textures->add(pda->texture(i)->texture());
	}

	// load all guns into a texturearray
	uint32_t startGuns = nbItems;
	guns = DarkForces::FileLFD::loadAnim("guns", "DFBRIEF");
	nbItems = guns->size();
	for (size_t i = 0; i < nbItems; i++) {
		m_textures->add(guns->texture(i)->texture());
	}

	// load all items into a texturearray
	uint32_t startItems = startGuns + nbItems;
	items = DarkForces::FileLFD::loadAnim("items", "DFBRIEF");
	nbItems = items->size();
	for (size_t i = 0; i < nbItems; i++) {
		m_textures->add(items->texture(i)->texture());
	}

	m_textures->generate();

	glm::vec4 texel;
	glm::vec4 texel_off;
	m_textures->texel(0, texel);
	GameEngine::UI_picture* ui_background = new GameEngine::UI_picture("DarkForces::pda::background", 
		glm::vec4(0, 0, 1, 1),
		texel);

	// tabs buttons
	m_textures->texel(2, texel);
	m_textures->texel(3, texel_off);
	GameEngine::UI_button* map = new GameEngine::UI_button(
		"DarkForces::pda::map",
		glm::vec4(75.0f / 320.0f, 176.0f / 200.0f, 27.0f / 320.0f, 14.0f / 200.0f),	
		texel_off,
		texel
	);

	m_textures->texel(4, texel);
	m_textures->texel(5, texel_off);
	GameEngine::UI_button* weapons = new GameEngine::UI_button(
		"DarkForces::pda::weapon",
		glm::vec4(115.0f/320.0f, 176.0f/200.0f, 31.0f/320.0f, 14.0f/200.0f),	
		texel_off,
		texel
	);

	m_textures->texel(6, texel);
	m_textures->texel(7, texel_off);
	GameEngine::UI_button* inv = new GameEngine::UI_button(
		"DarkForces::pda::inventory",
		glm::vec4(147.0f / 320.0f, 176.0f / 200.0f, 25.0f / 320.0f, 14.0f / 200.0f),	
		texel_off,
		texel
	);

	m_textures->texel(8, texel);
	m_textures->texel(9, texel_off);
	GameEngine::UI_button* obj = new GameEngine::UI_button(
		"DarkForces::pda::objects",
		glm::vec4(173.0f / 320.0f, 176.0f / 200.0f, 29.0f / 320.0f, 14.0f / 200.0f),	
		texel_off,
		texel
	);

	m_textures->texel(10, texel);
	m_textures->texel(11, texel_off);
	GameEngine::UI_button* mis = new GameEngine::UI_button(
		"DarkForces::pda::mission",
		glm::vec4(217.0f / 320.0f, 176.0f / 200.0f, 27.0f / 320.0f, 14.0f / 200.0f),	
		texel_off,
		texel
	);

	// TAB of buttons
	
	GameEngine::UI_tab* tab = new GameEngine::UI_tab(
		"DarkForces::pda::objects",
		//glm::vec4(70.0f / 320.0f, 176.0f / 200.0f, 175.0f / 320.0f, 14.0f / 200.0f)
		glm::vec4(0.0f / 320.0f, 0.0f / 200.0f, 320.0f / 320.0f, 200.0f / 200.0f)
		);

	// Exit button

	m_textures->texel(12, texel);
	m_textures->texel(13, texel_off);
	GameEngine::UI_button* exit = new GameEngine::UI_button(
		"DarkForces::pda::exit",
		glm::vec4(266.0f / 320.0f, 185.0f / 200.0f, 24.0f / 320.0f, 15.0f / 200.0f),	
		texel_off,
		texel,
		DarkForces::Message::PDA_EXIT
	);

	// build the tab for weapons
	m_ui_weapons = new GameEngine::UI_widget("DarkForces::pda::weapons",
		glm::vec4(0.06, 0.06, 0.9, 0.76)	// position & size of the tab panel
	);

	float x=0.0f, y=0.0f;
	for (size_t c = 0; c < guns->size(); c++) {
		m_textures->texel(startGuns + c, texel);
		GameEngine::UI_picture* ui_weapon = new GameEngine::UI_picture("DarkForces::pda::weapon",
			glm::vec4(x, y, 0.49f, 0.19f),
			texel,
			false	// hide weapons by default
		);	

		m_ui_weapons->add(ui_weapon);

		y += 0.2f;	// next row
		if (y >= 1.0f) {
			y = 0.0f;
			x += 0.5f;
		}
	}
	m_ui_weapons->widget(0)->visible(true);	// display the briad pistol

	m_root = ui_background;
		ui_background->add(tab);
			tab->add(map);
			tab->add(weapons);
			tab->add(inv);
			tab->add(obj);
			tab->add(mis);
		ui_background->add(exit);
		ui_background->add(m_ui_weapons);

	m_root->link(this);
	tab->tab(weapons);

	delete guns;
	delete items;
	delete pda;
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

		case DarkForces::Message::PDA_EXIT:
			onShowPDA(message);
			break;

		case gaMessage::Action::KEY:
			onKeyDown(message);
			break;

		case gaMessage::ADD_ITEM:
			onAddItem(message);
			break;
	}
	GameEngine::UI::dispatchMessage(message);
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
	}
}