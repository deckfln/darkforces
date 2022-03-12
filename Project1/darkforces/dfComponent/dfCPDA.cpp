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
static Framework::TextureAtlas* g_items_textures = nullptr;
static fwTexture* g_items_texture = nullptr;

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
DarkForces::Component::PDA::PDA(void):
	gaComponent(DF_COMPONENT_PDA)
{
	// preload the PDA images
	DarkForces::ANIM* pda = nullptr;
	DarkForces::ANIM* guns = nullptr;
	DarkForces::ANIM* items = nullptr;

	g_items_textures = new Framework::TextureAtlas();

	pda = DarkForces::FileLFD::loadAnim("pda", "MENU");
	size_t nbItems = pda->size();
	for (size_t i = 0; i < nbItems; i++) {
		g_items_textures->add(pda->texture(i)->texture());
	}

	// load all guns into a texturearray
	uint32_t startGuns = nbItems;
	guns = DarkForces::FileLFD::loadAnim("guns", "DFBRIEF");
	nbItems = guns->size();
	for (size_t i = 0; i < nbItems; i++) {
		g_items_textures->add(guns->texture(i)->texture());
	}

	// load all items into a texturearray
	uint32_t startItems = startGuns + nbItems;
	items = DarkForces::FileLFD::loadAnim("items", "DFBRIEF");
	nbItems = items->size();
	for (size_t i = 0; i < nbItems; i++) {
		g_items_textures->add(items->texture(i)->texture());
	}

	g_items_texture = g_items_textures->generate();

	m_ui = new GameEngine::UI("DarkForces::PDA", g_items_textures, false);
	glm::vec4 texel;
	g_items_textures->texel(0, texel);
	GameEngine::UI_picture* ui_background = new GameEngine::UI_picture("DarkForces::pda::background", 
		glm::vec4(0, 0, 1, 1),
		texel);

	g_items_textures->texel(4, texel);
	GameEngine::UI_picture* guns_ui = new GameEngine::UI_picture("DarkForces::pda::weapon",
		glm::vec4(115.0f/320.0f, 176.0f/200.0f, 31.0f/320.0f, 14.0f/200.0f),	// position & size of the button Weapon
		texel
	);

	// build the tab for weapons
	m_ui_weapons = new GameEngine::UI_widget("DarkForces::pda::weapons",
		glm::vec4(0.06, 0.06, 0.9, 0.76)	// position & size of the tab panel
	);

	float x=0.0f, y=0.0f;
	for (size_t c = 0; c < guns->size(); c++) {
		g_items_textures->texel(startGuns + c, texel);
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

	m_ui->root(ui_background);
		ui_background->add(guns_ui);
		ui_background->add(m_ui_weapons);

	delete guns;
	delete items;
	delete pda;
}

GameEngine::UI* DarkForces::Component::PDA::ui(void)
{
	return m_ui;
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