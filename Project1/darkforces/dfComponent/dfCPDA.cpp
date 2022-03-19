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

/**
 * when the playre reached a goal
 */
void DarkForces::Component::PDA::onCompleteGoal(gaMessage* message)
{
	// display the DELT od the given goal
	m_ui_goals->widget(message->m_value)->visible(true);
}

//------------------------------------------------------
// 
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
	m_textures->texel(pressed + 1, texel_off);
	return new GameEngine::UI_button(
		"DarkForces:pda:" + name,
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
	DarkForces::DELT* parent,
	const std::string& name,
	uint32_t pressed,
	uint32_t message
)
{
	// image position on the mega_texture
	glm::vec4 texel;
	glm::vec4 texel_off;

	// image position on screen (extracted from the DELT)
	const glm::vec2 size = m_pda->texture(pressed)->size();
	const glm::vec2 position = m_pda->texture(pressed)->position();

	// parent position
	const glm::vec2& parentPixelSize = parent->size();
	const glm::vec2& parentPixelOffset = parent->position();

	m_textures->texel(pressed, texel);
	m_textures->texel(pressed + 1, texel_off);
	return new GameEngine::UI_button(
		"DarkForces:pda:" + name,
		// convert "pixel absolute position" into "relative position against parent in %"
		glm::vec4((position.x - parentPixelOffset.x) / parentPixelSize.x, (position.y - parentPixelOffset.y) / parentPixelSize.y, size.x / parentPixelSize.x, size.y / parentPixelSize.y),
		texel_off,
		texel,
		message
	);
}

/**
 * add an image on the panel
 */
GameEngine::UI_picture* DarkForces::Component::PDA::addImage(
	DarkForces::DELT* parent,
	const std::string& name,
	DarkForces::ANIM* source,
	uint32_t image,
	uint32_t textureIndex
)
{
	glm::vec4 texel;

	m_textures->texel(textureIndex + image, texel);

	// image position on screen (extracted from the DELT)
	const glm::vec2 size = source->texture(image)->size();
	const glm::vec2 position = source->texture(image)->position();

	// parent position
	const glm::vec2& parentPixelSize = parent->size();
	const glm::vec2& parentPixelOffset = parent->position();

	return new GameEngine::UI_picture(
		"DarkForces:pda:" + name,
		glm::vec4((position.x - parentPixelOffset.x) / parentPixelSize.x, (position.y - parentPixelOffset.y) / parentPixelSize.y, size.x / parentPixelSize.x, size.y / parentPixelSize.y),
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
	m_pda_position = m_pda->texture(0)->position();
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
		glm::vec4(0.0f / m_pda_size.x, position.y / m_pda_size.y, 320.0f / m_pda_size.x, size.y / m_pda_size.y)	// tab buttons
	);

	GameEngine::UI_button* map = buildButton("map", PDA_BUTTONS::Map_press, 0);
	GameEngine::UI_button* weapons = buildButton("weapon", PDA_BUTTONS::Weapon_press, 0);
	GameEngine::UI_button* inv = buildButton("inventory", PDA_BUTTONS::Inventory_press, 0);
	GameEngine::UI_button* obj = buildButton("objects", PDA_BUTTONS::Object_press, 0);
	GameEngine::UI_button* mis = buildButton("mission", PDA_BUTTONS::Mission_press, 0);

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
			addImage(
				m_pda->texture(1),
				"weapon",
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
			addImage(
				m_pda->texture(1),
				"item",
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
		m_pda->texture(1),
		"scrollup", 
		PDA_BUTTONS::Up_press, 
		DarkForces::Message::PDA_UP
	);

	GameEngine::UI_button* scrolldown = addButton(
		m_pda->texture(1),
		"scrolldown",
		PDA_BUTTONS::Down_press, 
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

	// add the completed goals
	for (size_t i = 1; i < goals->size(); i++) {
		m_ui_goals->add(
			addImage(
				goals->texture(0),
				"goal",
				goals,
				i,
				pgoals
			)
		);
	}
	goal_tab->add(m_ui_goals);

	// -------------------------- Exit button
	GameEngine::UI_button* exit = addButton(
		m_pda->texture(0),
		"exit",
		PDA_BUTTONS::Exit_press,
		DarkForces::Message::PDA_EXIT
	);

	// -------------------------- final build
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

		case DarkForces::Message::COMPLETE:
			onCompleteGoal(message);
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