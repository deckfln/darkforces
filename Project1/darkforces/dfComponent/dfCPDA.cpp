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

#include "../../framework/fwScene.h"

#include "../dfComponent.h"
#include "../dfMessage.h"


//-------------------------------------------

static std::map<uint32_t, std::string> g_dfWeaponsRev = {
	{0, "pistol"},
	{1, "rifle"},
	{2, "detonator"},
	{3, "repeater"},
	{4, "fusion"},
	{5, "mine"},
	{6, "mortar"},
	{7, "concussion"},
	{8, "canoon"},
};

static std::map<uint32_t, std::string> g_dfInventoryRev = {
	{0, "redkey"},
	{1, "bluekey"},
	{2, "yellowkey"},
	{9, "datatape"},
	{6, "plans"},
	{11,"dtweapon"},
	{7, "phrik"},
	{8, "nava"},
	{4, "icecleats"},
	{5, "gasmask"},
	{3, "goggles"},
	{12,"code1"},
	{13,"code2"},
	{14,"code3"},
	{15,"code4"},
	{16,"code5"}
};

//-------------------------------------------

/**
 * display the PDA
 */
void DarkForces::Component::PDA::onShowPDA(gaMessage*)
{
	if (m_background->visible()) {
		m_background->visible(false);
		GameEngine::World::popState();				// restart the game
		GameEngine::App::popControl();				// restore the correct controler
	}
	else {
		m_background->visible(true);
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
	GameEngine::Image2D* image = m_ui_weapons->widget(item->name());
	if (image != nullptr) {
		image->visible(true);
	}

	image = m_ui_inventory->widget(item->name());
	if (image) {
		image->visible(true);
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
	// display the DELT of the given goal
	__debugbreak();
	m_ui_goals->widget("DarkForces:goal:" + std::to_string(message->m_value))->visible(true);
}

/**
 * set the new screen ratio
 */
void DarkForces::Component::PDA::onScreenResize(gaMessage* message)
{
	if (m_ratio != message->m_fvalue) {
		// the PDA was originaly designed for 320x200 ratio
		m_ratio = message->m_fvalue;
		m_background->scale(glm::vec2(1.6f / m_ratio, 1.0f));
	}
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
	Left_press,
	Left_Release,
	Zoom_Down_press,
	Zoom_Down_Release,
	Zoom_Up_press,
	Zoom_Up_Release,
	Floor_Up_press,
	Floor_Up_release,
	Floor_Down_press,
	Floor_Down_release
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

	const glm::vec2 scale(
		size.x / 320.0f,
		size.y / 200.0f
	);
	const glm::vec2 translation(
		(position.x + size.x / 2.0f) / 160.0f - 1.0f,
		1.0f - (position.y + size.y / 2.0f) / 100.0f
	);

	m_textures->texel(pressed, texel);
	m_textures->texel(pressed + 1, texel_off);
	GameEngine::UI_button *button = new GameEngine::UI_button(
		"DarkForces:pda:" + name,
		scale,
		translation,
		texel_off,
		texel,
		m_textures->texture(),
		message
	);

	return button;
}

/**
 * add a button on a panel
 */
void DarkForces::Component::PDA::completeButtonDef(
	DarkForces::DELT* parent,
	GameEngine::UI_def_button& button
)
{
	// image position on the mega_texture
	glm::vec4 texel;
	glm::vec4 texel_off;

	// image position on screen (extracted from the DELT)
	const glm::vec2 size = m_pda->texture(button.imageIndex)->size();
	const glm::vec2 position = m_pda->texture(button.imageIndex)->position();

	// parent position
	const glm::vec2& parentPixelSize = parent->size();
	const glm::vec2& parentPixelOffset = parent->position();

	m_textures->texel(button.imageIndex, texel);
	m_textures->texel(button.imageIndex + 1, texel_off);

	button.scale = glm::vec2(
		size.x / 320.0f,
		size.y / 200.0f
	);
	button.translation = glm::vec2(
		(position.x + size.x / 2.0f) / 160.0f - 1.0f,
		1.0f - (position.y + size.y / 2.0f) / 100.0f
	);
	button.img_press_position_size = texel;
	button.img_release_position_size = texel_off;
	button.name = "DarkForces:pda:" + std::string(button.name);
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

	const glm::vec2 scale(
		size.x / parentPixelSize.x,
		size.y / parentPixelSize.y
	);
	const glm::vec2 translation(
		(position.x - parentPixelOffset.x + size.x / 2.0f) / (parentPixelSize.x / 2.0f) - 1.0f,
		1.0f - (position.y - parentPixelOffset.y + size.y / 2.0f) / (parentPixelSize.y / 2.0f)
	);

	GameEngine::UI_picture* picture = new GameEngine::UI_picture(
		name,
		scale,
		translation,
		texel,
		m_textures->texture(),
		false	// hide weapons by default
	);

	return picture;
}

//------------------------------------------------------

/**
 * create
 */
DarkForces::Component::PDA::PDA(const std::string& name):
	GameEngine::UI(name)
{
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

	fwTexture* textureAtlas = m_textures->texture();

	//setTexture(m_textures->texture());

	//m_textures->save("0.png");

	// build the GUI ------------------------------------

	glm::vec2 size;
	glm::vec2 position;

	glm::vec4 texel;

	m_textures->texel(PDA_BUTTONS::Background, texel);
	m_background = new GameEngine::UI_picture(
		"DarkForces::pda::background", 
		glm::vec2(1),	// scale
		glm::vec2(0),	// translate
		texel,
		textureAtlas,
		false);

	// TAB of buttons
	size = m_pda->texture(PDA_BUTTONS::Map_press)->size();
	position = m_pda->texture(PDA_BUTTONS::Map_press)->position();

	GameEngine::UI_tab* tab = new GameEngine::UI_tab(
		"DarkForces::pda::tab",
		glm::vec2(1),	// scale
		glm::vec2(0)	// translate
	);

	GameEngine::UI_button* map = buildButton("map", PDA_BUTTONS::Map_press, 0);
	GameEngine::UI_button* weapons = buildButton("weapon", PDA_BUTTONS::Weapon_press, 0);
	GameEngine::UI_button* inv = buildButton("inventory", PDA_BUTTONS::Inventory_press, 0);
	GameEngine::UI_button* obj = buildButton("objects", PDA_BUTTONS::Object_press, 0);
	GameEngine::UI_button* mis = buildButton("mission", PDA_BUTTONS::Mission_press, 0);

	// -------------------- build the tab for weapons
	const glm::vec2 scale(
		m_pda_panel_size.x / 320.0f,
		m_pda_panel_size.y / 200.0f
	);
	const glm::vec2 translation(
		(m_pda_panel_position.x + m_pda_panel_size.x / 2.0f) / 160.0f - 1.0f,
		1.0f - (m_pda_panel_position.y + m_pda_panel_size.y / 2.0f) / 100.0f
	);

	m_ui_weapons = new GameEngine::UI_container(
		"DarkForces::pda::weapons",
		scale,
		translation
		);

	float x = 0.0f, y = 0.0f;
	for (size_t c = 0; c < guns->size(); c++) {
		m_ui_weapons->add(
			addImage(
				m_pda->texture(1),
				"DarkForces:weapon:" + g_dfWeaponsRev[c],
				guns,
				c,
				startGuns
			)
		);
	}

	// -------------------- build the tab for inventory
	m_ui_inventory = new GameEngine::UI_container(
		"DarkForces::pda::items",
		scale,
		translation
	);

	uint32_t c = 0;
	float w = 0;

	for (size_t i = 0; i < items->size(); i++) {
		m_ui_inventory->add(
			addImage(
				m_pda->texture(1),
				"DarkForces:item:" + g_dfInventoryRev[i],
				items,
				i,
				startItems
			)
		);
	}

	// -------------------- build the tab for mission
	GameEngine::UI_container* mission_tab = new GameEngine::UI_container(
		"DarkForces::pda::mis_panel",
		scale,
		translation
	);

	m_textures->texel(briefing, texel);
	size = secbase->size();
	position = secbase->position();
	m_ui_debrief = new GameEngine::UI_ZoomPicture(
		"DarkForces::pda::missions",
		glm::vec2(size.x / m_pda_panel_size.x, 1.0f),	// scale
		glm::vec2(0),									// translation
		texel,
		size,
		glm::ivec2(0,155),
		textureAtlas
	);
	mission_tab->add(m_ui_debrief);

	static std::vector<struct UI_def_button> mission_buttons = {
		{"scrollup", 	PDA_BUTTONS::Up_press,	DarkForces::Message::PDA_UP,	true, false, glm::vec4(0),	glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"scrolldown",	PDA_BUTTONS::Down_press,DarkForces::Message::PDA_DOWN,	true, false, glm::vec4(0),	glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)}
	};

	// convert DarkForces UI to GameEngine::UI
	for (auto& button : mission_buttons) {
		completeButtonDef(m_pda->texture(1), button);
	}
	mission_tab->addButtons(mission_buttons);

	// -------------------- build the tab for goals
	GameEngine::UI_container* goal_tab = new GameEngine::UI_container(
		"DarkForces::pda::goal_panel",
		scale,
		translation
	);

	m_textures->texel(pgoals, texel);
	size = goals->texture(0)->size();
	position = goals->texture(0)->position();
	m_ui_goals = new GameEngine::UI_picture(
		"DarkForces::pda::goals",
		size / m_pda_panel_size,	// scale
		glm::vec2(0),				// translation
		texel,
		textureAtlas
	);

	// add the completed goals
	for (size_t i = 1; i < goals->size(); i++) {
		m_ui_goals->add(
			addImage(
				goals->texture(0),
				"DarkForces:goal:"+std::to_string(i),
				goals,
				i,
				pgoals
			)
		);
	}
	goal_tab->add(m_ui_goals);

	// -------------------- build the tab for automap
	GameEngine::UI_container* map_tab = new GameEngine::UI_container(
		"DarkForces::pda::map_panel",
		scale,
		translation
	);

	std::vector<struct UI_def_button> automap_buttons = {
		{"up",			PDA_BUTTONS::Up_press,			DarkForces::Message::PDA_UP,		true,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"down",		PDA_BUTTONS::Down_press,		DarkForces::Message::PDA_DOWN,		true,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"left",		PDA_BUTTONS::Left_press,		DarkForces::Message::PDA_LEFT,		true,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"right",		PDA_BUTTONS::Right_press,		DarkForces::Message::PDA_RIGHT,		true,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"zoomdown",	PDA_BUTTONS::Zoom_Down_press,	DarkForces::Message::PDA_ZOOM_DOWN,	true,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"zoomup",		PDA_BUTTONS::Zoom_Up_press,		DarkForces::Message::PDA_ZOOM_UP,	true,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"floordown",	PDA_BUTTONS::Floor_Down_press,	DarkForces::Message::PDA_FLOOR_DOWN,false,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)},
		{"floorup",		PDA_BUTTONS::Floor_Up_press,	DarkForces::Message::PDA_FLOOR_UP,	false,	false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)}
	};

	for (auto& button : automap_buttons) {
		completeButtonDef(m_pda->texture(1), button);
	}
	map_tab->addButtons(automap_buttons);

	// -------------------------- Exit button
	static std::vector<struct UI_def_button> pda_buttons = {
		{"exit", 		PDA_BUTTONS::Exit_press,		DarkForces::Message::PDA_EXIT,		false, false, glm::vec4(0),glm::vec4(0), textureAtlas, glm::vec2(0), glm::vec2(0)}
	};

	completeButtonDef(m_pda->texture(0), pda_buttons[0]);
	m_background->addButtons(pda_buttons);

	// -------------------------- final build
	m_root = m_background;
		m_background->addChild(tab);
			tab->addTab(map, map_tab);
			tab->addTab(weapons, m_ui_weapons);
			tab->addTab(inv, m_ui_inventory);
			tab->addTab(obj, goal_tab);
			tab->addTab(mis, mission_tab);

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

		case gaMessage::Action::SCREEN_RESIZE:
			onScreenResize(message);
			break;

	}
	GameEngine::UI::dispatchMessage(message);
}

/**
 *
 */
Framework::Mesh2D* DarkForces::Component::PDA::ui(void)
{
	return m_background;
}

#ifdef _DEBUG
/**
 *
 */
void DarkForces::Component::PDA::debugGUIinline(void)
{
	if (ImGui::TreeNode("dfCPDA")) {
		ImGui::TreePop();
	}
}
#endif

/**
 *
 */
DarkForces::Component::PDA::~PDA(void)
{
	if (m_pda != nullptr) {
		delete m_pda;
		delete m_guns;
		delete m_items;
		delete m_pda_background;
	}
}