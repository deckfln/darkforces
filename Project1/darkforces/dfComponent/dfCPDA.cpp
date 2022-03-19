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


//-------------------------------------------

static std::map<std::string, uint32_t> g_dfItems = {
	{"Pistol", 0},
	{"Rifle", 1},
	{"Detonator", 2},
	{"Repeater", 3},
	{"Fusion", 4},
	{"Mine", 5},
	{"Mortar", 6},
	{"Concussion", 7},
	{"Canoon", 8},
};

static std::map<std::string, uint32_t> g_dfInventory = {
	{ "darkforce:redkey", 0},
	{ "darkforce:bluekey", 1},
	{ "darkforce:yellowkey", 2},
	{ "darkforce:datatape", 9},
	{ "darkforce:plans", 6},
	{ "darkforce:dtweapon", 11},
	{ "darkforce:phrik", 7},
	{ "darkforce:nava", 8},
	{ "darkforce:icecleats", 4},
	{ "darkforce:gasmask", 5},
	{ "darkforce:goggles", 3},
	{ "darkforce:code1", 12},
	{ "darkforce:code2", 13},
	{ "darkforce:code3", 14},
	{ "darkforce:code4", 15},
	{ "darkforce:code5", 16}
};

//-------------------------------------------

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
	if (g_dfInventory.count(item->name()) > 0) {
		m_ui_inventory->widget(g_dfInventory[item->name()])->visible(true);
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
 * add a button on the tab
 */
GameEngine::UI_button* DarkForces::Component::PDA::buildButton(
	const std::string& name,
	uint32_t pressed,
	uint32_t released,
	uint32_t message
)
{
	// image position on the mega_texture
	glm::vec4 texel;
	glm::vec4 texel_off;

	// image position on screen (extracted from the DELT)
	const glm::vec2 size = m_pda->texture(pressed)->size();
	const glm::vec2 position = m_pda->texture(pressed)->position();

	m_textures->texel(pressed, texel);
	m_textures->texel(released, texel_off);
	return new GameEngine::UI_button(
		name,
		glm::vec4(position.x / m_pda_size.x, 0.0, size.x / m_pda_size.x, 1.0),
		texel_off,
		texel,
		message
	);
}

/**
 * add a button on a panel
 */
GameEngine::UI_button* DarkForces::Component::PDA::addButton(
	const glm::vec2& parentPixelSize,
	const std::string& name,
	uint32_t pressed,
	uint32_t released,
	uint32_t message
)
{
	// image position on the mega_texture
	glm::vec4 texel;
	glm::vec4 texel_off;

	// image position on screen (extracted from the DELT)
	const glm::vec2 size = m_pda->texture(pressed)->size();
	const glm::vec2 position = m_pda->texture(pressed)->position();

	m_textures->texel(pressed, texel);
	m_textures->texel(released, texel_off);
	return new GameEngine::UI_button(
		name,
		glm::vec4((position.x - m_pda_panel_position.x) / parentPixelSize.x, (position.y - m_pda_panel_position.y) / parentPixelSize.y, size.x / parentPixelSize.x, size.y / parentPixelSize.y),
		texel_off,
		texel,
		message
	);
}

/**
 * add an image on the panel
 */
GameEngine::UI_picture* DarkForces::Component::PDA::buidIcon(
	const std::string& name,
	DarkForces::ANIM* source,
	uint32_t image,
	uint32_t textureIndex
)
{
	glm::vec4 texel;

	m_textures->texel(textureIndex + image, texel);
	const glm::vec2 size = source->texture(image)->size();
	const glm::vec2 position = source->texture(image)->position();

	return new GameEngine::UI_picture(
		name,
		glm::vec4((position.x - m_pda_panel_position.x) / m_pda_panel_size.x, (position.y - m_pda_panel_position.y) / m_pda_panel_size.y, size.x / m_pda_panel_size.x, size.y / m_pda_panel_size.y),
		texel,
		false	// hide weapons by default
	);
}

//------------------------------------------------------

/**
 * create
 */
DarkForces::Component::PDA::PDA(const std::string& name):
	GameEngine::UI(name)
{
	m_visible = false;

	DarkForces::ANIM* guns = nullptr;
	DarkForces::ANIM* items = nullptr;
	DarkForces::ANIM* goals = nullptr;
	DarkForces::DELT* secbase = nullptr;	//secbase briefing

	m_textures = new Framework::TextureAtlas();

	
	m_pda = DarkForces::FileLFD::loadAnim("pda", "MENU");
	size_t nbItems = m_pda->size();
	for (size_t i = 0; i < nbItems; i++) {
		m_textures->add(m_pda->texture(i)->texture());
	}

	m_pda_size = m_pda->texture(0)->size();
	m_pda_panel_size = m_pda->texture(1)->size();
	m_pda_panel_position = m_pda->texture(1)->position();

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

	// add secbase goals
	uint32_t pgoals = briefing + 1;
	goals = DarkForces::FileLFD::loadAnim("secbase", "DFBRIEF");
	nbItems = goals->size();
	for (size_t i = 0; i < nbItems; i++) {
		m_textures->add(goals->texture(i)->texture());
	}

	m_textures->generate();
	//m_textures->save("0.png");

	// build the GUI ------------------------------------

	glm::vec2 size;
	glm::vec2 position;

	glm::vec4 texel;
	glm::vec4 texel_off;

	m_textures->texel(PDA_BUTTONS::Background, texel);
	GameEngine::UI_picture* ui_background = new GameEngine::UI_picture(
		"DarkForces::pda::background", 
		glm::vec4(0, 0, 1, 1),
		texel);

	// TAB of buttons
	size = m_pda->texture(PDA_BUTTONS::Map_press)->size();
	position = m_pda->texture(PDA_BUTTONS::Map_press)->position();

	GameEngine::UI_tab* tab = new GameEngine::UI_tab(
		"DarkForces::pda::tab",
		glm::vec4(0.0f / 320.0f, position.y / m_pda_size.y, 320.0f / 320.0f, size.y / m_pda_size.y)	// tab buttons
	);

	GameEngine::UI_button* map = buildButton("DarkForces::pda::map", PDA_BUTTONS::Map_press, PDA_BUTTONS::Map_release, 0);
	GameEngine::UI_button* weapons = buildButton("DarkForces::pda::weapon", PDA_BUTTONS::Weapon_press, PDA_BUTTONS::Weapon_release, 0);
	GameEngine::UI_button* inv = buildButton("DarkForces::pda::inventory", PDA_BUTTONS::Inventory_press, PDA_BUTTONS::Inventory_release, 0);
	GameEngine::UI_button* obj = buildButton("DarkForces::pda::objects", PDA_BUTTONS::Object_press, PDA_BUTTONS::Object_release, 0);
	GameEngine::UI_button* mis = buildButton("DarkForces::pda::mission", PDA_BUTTONS::Mission_press, PDA_BUTTONS::Mission_release, 0);

	// TAB of buttons
	GameEngine::UI_widget* panel = new GameEngine::UI_widget(
		"DarkForces::pda::panel",
		glm::vec4(m_pda_panel_position.x / m_pda_size.x, m_pda_panel_position.y / m_pda_size.y, m_pda_panel_size.x / m_pda_size.x, m_pda_panel_size.y / m_pda_size.y)	// position & size of the tab panel
		);

	// -------------------- build the tab for weapons
	m_ui_weapons = new GameEngine::UI_widget(
		"DarkForces::pda::weapons",
		glm::vec4(0.0, 0.0, 1.0, 1.0)
		);

	float x = 0.0f, y = 0.0f;
	for (size_t c = 0; c < guns->size(); c++) {
		m_ui_weapons->add(
			buidIcon(
				"DarkForces::pda::weapon",
				guns,
				c,
				startGuns
			)
		);
	}

	// -------------------- build the tab for inventory
	m_ui_inventory = new GameEngine::UI_widget(
		"DarkForces::pda::items",
		glm::vec4(0.0, 0.0, 1.0, 1.0)
	);

	uint32_t c = 0;
	float w = 0;

	for (size_t i = 0; i < items->size(); i++) {
		m_ui_inventory->add(
			buidIcon(
				"DarkForces::pda::item",
				items,
				i,
				startItems
			)
		);
	}

	// -------------------- build the tab for mission
	GameEngine::UI_widget* mission_tab = new GameEngine::UI_widget(
		"DarkForces::pda::panel",
		glm::vec4(0.0, 0.0, 1.0, 1.0)	// position & size of the tab panel
	);

	m_textures->texel(briefing, texel);
	size = secbase->size();
	position = secbase->position();
	m_ui_debrief = new GameEngine::UI_ZoomPicture(
		"DarkForces::pda::missions",
		glm::vec4((1.0 - size.x / m_pda_panel_size.x) / 2.0f, 0.0, size.x / m_pda_panel_size.x, 1.0),
		texel,
		size,
		glm::ivec2(0,155)
	);

	GameEngine::UI_button* scrollup = addButton(
		m_pda_panel_size,
		"DarkForces::pda::scrollup", 
		PDA_BUTTONS::Up_press, PDA_BUTTONS::Up_release, 
		DarkForces::Message::PDA_UP
	);

	GameEngine::UI_button* scrolldown = addButton(
		m_pda_panel_size,
		"DarkForces::pda::scrolldown",
		PDA_BUTTONS::Down_press, PDA_BUTTONS::Down_release, 
		DarkForces::Message::PDA_DOWN
	);

	mission_tab->add(m_ui_debrief);
	mission_tab->add(scrollup);
	mission_tab->add(scrolldown);

	// -------------------- build the tab for goals
	GameEngine::UI_widget* goal_tab = new GameEngine::UI_widget(
		"DarkForces::pda::panel",
		glm::vec4(0.0, 0.0, 1.0, 1.0)	// position & size of the tab panel
	);

	m_textures->texel(pgoals, texel);
	size = goals->texture(0)->size();
	position = goals->texture(0)->position();
	m_ui_goals = new GameEngine::UI_picture(
		"DarkForces::pda::goals",
		glm::vec4((1.0 - size.x / m_pda_panel_size.x) / 2.0f, 0.0, size.x / m_pda_panel_size.x, size.y / m_pda_panel_size.y),
		texel
	);

	goal_tab->add(m_ui_goals);

	// -------------------------- Exit button
	m_textures->texel(PDA_BUTTONS::Exit_press, texel);
	m_textures->texel(PDA_BUTTONS::Exit_release, texel_off);
	size = m_pda->texture(PDA_BUTTONS::Exit_press)->size();
	position = m_pda->texture(PDA_BUTTONS::Exit_press)->position();
	GameEngine::UI_button* exit = new GameEngine::UI_button(
		"DarkForces::pda::exit",
		//glm::vec4(266.0f / 320.0f, 185.0f / 200.0f, 24.0f / 320.0f, 15.0f / 200.0f),	
		glm::vec4(position.x / m_pda_size.x, position.y / m_pda_size.y, size.x / m_pda_size.x, size.y / m_pda_size.y),
		texel_off,
		texel,
		DarkForces::Message::PDA_EXIT
	);

	m_root = ui_background;
		ui_background->add(tab);
			tab->setPanel(panel);
			tab->addTab(map, nullptr);
			tab->addTab(weapons, m_ui_weapons);
			tab->addTab(inv, m_ui_inventory);
			tab->addTab(obj, m_ui_goals);
			tab->addTab(mis, mission_tab);
		ui_background->add(panel);
		ui_background->add(exit);

	m_root->link(this);
	tab->tab(weapons);	// force the tab original panel

	delete secbase;
	delete guns;
	delete items;
	delete m_pda;
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