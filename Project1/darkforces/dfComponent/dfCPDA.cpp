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

// list of pictures in the PDA MENU ADMIN
enum PDA_BUTTONS {
	Background,
	XXX,
	Map_press,
	Map_release,
	Weapon_press,
	Weapon_release,
	Inventory_press,
	Inventory_release,
	Object_press,
	Object_release,
	Mission_press,
	Mission_release,
	Exit_press,
	Exit_release,
	Up_press,
	Up_release,
	Right_press,
	Right_release,
	Down_press,
	Down_release,
};

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
	DarkForces::DELT* secbase = nullptr;	//secbase briefing

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

	// add secbase briefing
	uint32_t briefing = startItems + nbItems;
	secbase = DarkForces::FileLFD::loadDelt("secbase", "DFBRIEF");
	m_textures->add(secbase->texture());

	m_textures->generate();
	//m_textures->save("0.png");

	glm::vec4 texel;
	glm::vec4 texel_off;
	m_textures->texel(PDA_BUTTONS::Background, texel);
	GameEngine::UI_picture* ui_background = new GameEngine::UI_picture("DarkForces::pda::background", 
		glm::vec4(0, 0, 1, 1),
		texel);

	// tabs buttons
	m_textures->texel(PDA_BUTTONS::Map_press, texel);
	m_textures->texel(PDA_BUTTONS::Map_release, texel_off);
	GameEngine::UI_button* map = new GameEngine::UI_button(
		"DarkForces::pda::map",
		glm::vec4(75.0f / 320.0f, 0.0f, 27.0f / 320.0f, 1.0f),	
		texel_off,
		texel
	);

	m_textures->texel(PDA_BUTTONS::Weapon_press, texel);
	m_textures->texel(PDA_BUTTONS::Weapon_release, texel_off);
	GameEngine::UI_button* weapons = new GameEngine::UI_button(
		"DarkForces::pda::weapon",
		glm::vec4(115.0f/320.0f, 0.0f, 31.0f/320.0f, 1.0f),	
		texel_off,
		texel
	);

	m_textures->texel(PDA_BUTTONS::Inventory_press, texel);
	m_textures->texel(PDA_BUTTONS::Inventory_release, texel_off);
	GameEngine::UI_button* inv = new GameEngine::UI_button(
		"DarkForces::pda::inventory",
		glm::vec4(147.0f / 320.0f, 0.0f, 25.0f / 320.0f, 1.0f),	
		texel_off,
		texel
	);

	m_textures->texel(PDA_BUTTONS::Object_press, texel);
	m_textures->texel(PDA_BUTTONS::Object_release, texel_off);
	GameEngine::UI_button* obj = new GameEngine::UI_button(
		"DarkForces::pda::objects",
		glm::vec4(173.0f / 320.0f, 0.0f, 29.0f / 320.0f, 1.0f),	
		texel_off,
		texel
	);

	m_textures->texel(PDA_BUTTONS::Mission_press, texel);
	m_textures->texel(PDA_BUTTONS::Mission_release, texel_off);
	GameEngine::UI_button* mis = new GameEngine::UI_button(
		"DarkForces::pda::mission",
		glm::vec4(217.0f / 320.0f, 0.0f, 27.0f / 320.0f, 1.0f),	
		texel_off,
		texel
	);

	// TAB of buttons
	GameEngine::UI_tab* tab = new GameEngine::UI_tab(
		"DarkForces::pda::tab",
		glm::vec4(0.0f / 320.0f, 176.0f / 200.0f, 320.0f / 320.0f, 14.0f / 200.0f)	// tab buttons
		);

	GameEngine::UI_widget* panel = new GameEngine::UI_widget("DarkForces::pda::panel",
		glm::vec4(0.07, 0.07, 0.88, 0.74)	// position & size of the tab panel
		);

	// build the tab for weapons
	m_ui_weapons = new GameEngine::UI_widget("DarkForces::pda::weapons",
		glm::vec4(0.0, 0.0, 1.0, 1.0)
		);

	float x = 0.0f, y = 0.0f;
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

	// build the tab for mission
	GameEngine::UI_widget* mission_tab = new GameEngine::UI_widget("DarkForces::pda::panel",
		glm::vec4(0.0, 0.0, 1.0, 1.0)	// position & size of the tab panel
	);

	m_textures->texel(briefing, texel);
	m_ui_debrief = new GameEngine::UI_ZoomPicture(
		"DarkForces::pda::missions",
		glm::vec4(40.0f / 280.0f, 0.0, 200.0 / 280.0f, 155.0 / 155.0f),
		texel,
		secbase->size(),
		glm::ivec2(0,155)
	);

	m_textures->texel(PDA_BUTTONS::Up_press, texel);
	m_textures->texel(PDA_BUTTONS::Up_release, texel_off);
	GameEngine::UI_button* scrollup = new GameEngine::UI_button(
		"DarkForces::pda::scrollup",
		glm::vec4(243.0f / 280.0f, 9.0f / 155.0f, 20.0f / 280.0f, 15.0f / 155.0f),
		texel_off,
		texel,
		DarkForces::Message::PDA_UP
	);

	m_textures->texel(PDA_BUTTONS::Down_press, texel);
	m_textures->texel(PDA_BUTTONS::Down_release, texel_off);
	GameEngine::UI_button* scrolldown = new GameEngine::UI_button(
		"DarkForces::pda::scrolldown",
		glm::vec4(243.0f / 280.0f, 32.0f / 155.0f, 20.0f / 280.0f, 15.0f / 155.0f),
		texel_off,
		texel,
		DarkForces::Message::PDA_DOWN
	);

	mission_tab->add(m_ui_debrief);
	mission_tab->add(scrollup);
	mission_tab->add(scrolldown);

	// Exit button
	m_textures->texel(PDA_BUTTONS::Exit_press, texel);
	m_textures->texel(PDA_BUTTONS::Exit_release, texel_off);
	GameEngine::UI_button* exit = new GameEngine::UI_button(
		"DarkForces::pda::exit",
		glm::vec4(266.0f / 320.0f, 185.0f / 200.0f, 24.0f / 320.0f, 15.0f / 200.0f),	
		texel_off,
		texel,
		DarkForces::Message::PDA_EXIT
	);

	m_root = ui_background;
		ui_background->add(tab);
			tab->setPanel(panel);
			tab->addTab(map, nullptr);
			tab->addTab(weapons, m_ui_weapons);
			tab->addTab(inv, nullptr);
			tab->addTab(obj, nullptr);
			tab->addTab(mis, mission_tab);
		ui_background->add(panel);
		ui_background->add(exit);

	m_root->link(this);
	tab->tab(weapons);	// force the tab original panel

	delete secbase;
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

		case DarkForces::Message::PDA_UP:
			m_ui_debrief->scrollUp();
			break;

		case DarkForces::Message::PDA_DOWN:
			m_ui_debrief->scrollDown();
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