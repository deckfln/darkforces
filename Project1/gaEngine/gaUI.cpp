#include "gaUI.h"
#include "gaUI.h"
#include "gaUI.h"

#include <map>

#include "../glEngine/glUniformBuffer.h"

#include "../framework/fwHUDelement.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwUniform.h"
#include "../framework/fwMaterial.h"
#include "../framework/fwFlatPanel.h"

#include "gaEntity.h"
#include "World.h"

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "gaEngine/shaders/gui_vs.glsl"},
	{FRAGMENT_SHADER, "gaEngine/shaders/gui_fs.glsl"}
};

static fwFlatPanel* g_hudPanel = nullptr;
static fwMaterial g_material(g_subShaders);
static fwUniform* g_uniformTexture = nullptr;
static glm::vec4 g_screen;

//-------------------------------------------------------

static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	1.0f,  1.0f,
	1.0f, 1.0f,
	 1.0f, 1.0f,

	1.0f,  1.0f,
	 1.0f, 1.0f,
	 1.0f,  1.0f
};

static float quadUvs[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f
};

static glProgram* g_program = nullptr;
static fwGeometry* g_geometry = nullptr;
static glVertexArray* g_vertexArray = nullptr;
static glm::vec4 g_positionsize;
static glm::vec4 g_imagepos;
static fwUniform g_uni_ps("positionsize", &g_positionsize);
static fwUniform g_uni_ip("imagepos", &g_imagepos);
static fwUniform g_uni_tr("transformation", &g_positionsize);

//-------------------------------------------------------

/**
 * dipatch mouse actions to widgets
 */
void GameEngine::UI::onMouseDown(gaMessage* message)
{
	// convert from 0:1 space to -1:1 space
	const glm::vec2 glCursor = glm::vec2(message->m_v3value.x, -message->m_v3value.y) * 2.0f - glm::vec2(1, -1);
	uint32_t button = message->m_value;

	// find the widget that is under the cursor
	m_currentButton = m_root->findWidgetAt(glCursor);
	if (m_currentButton != nullptr) {
		m_currentButton->onMouseDown();
	}
}

void GameEngine::UI::onMouseUp(gaMessage*)
{
	if (m_currentButton) {
		m_currentButton->onMouseUp();
		m_currentButton = nullptr;
	}
}

/**
 *
 */
void GameEngine::UI::onMouseMove(gaMessage* message)
{
	const glm::vec2 glCursor = glm::vec2(message->m_v3value.x, -message->m_v3value.y) * 2.0f - glm::vec2(1, -1);
	UI_widget* m_current = m_root->findWidgetAt(glCursor);

	if (m_currentButton != m_current) {
		// moving to a new button
		if (m_currentButton != nullptr) {
			// inform the previous button that we exit its area
			m_currentButton->onExitArea();
		}

		if (m_current != nullptr) {
			// inform the new button that we enter its area
			m_current->onEnterArea();
		}

		m_currentButton = m_current;
	}

	if (m_currentButton != nullptr) {
		m_currentButton->onMouseMove();
	}
}

/**
 *
 */
void GameEngine::UI::onTimer(gaMessage*)
{
	if (m_currentButton) {
		m_currentButton->onTimer();
	}
}

/**
 * create an UI
 */
GameEngine::UI::UI(const std::string& name, bool visible):
	gaComponent(gaComponent::Gui)
{
	m_visible = true; //  visible;

	g_material.addTexture("image", (fwTexture*)nullptr);
	g_material.addUniform(&g_uni_ps);
	g_material.addUniform(&g_uni_ip);
	g_material.addUniform(&g_uni_tr);

	std::string vs = g_material.load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
	std::string fs = g_material.load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
	std::string gs = g_material.load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

	g_program = new glProgram(vs, fs, gs, "");

	g_geometry = new fwGeometry();
	g_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
	g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);

	g_vertexArray = new glVertexArray();
	g_geometry->enable_attributes(g_program);
	g_vertexArray->unbind();
}

/**
 * draw the GUI
 */
void GameEngine::UI::draw(void)
{
	if (!m_visible) {
		return;
	}

	if (m_dirty) {
		m_dirty = false;
		m_root->update(glm::vec4(0, 0, 1, 1));
	}
	// always draw on top of screen
	glDisable(GL_DEPTH_TEST);								// disable depth test so screen-space quad isn't discarded due to depth test.

	g_program->run();

	m_root->draw();
	glEnable(GL_DEPTH_TEST);
}

/**
 * draw a widget
 */
void GameEngine::UI::draw_widget(const glm::vec4& position_size)
{
	g_positionsize = position_size;
	g_material.set_uniforms(g_program);
	g_geometry->draw(GL_TRIANGLES, g_vertexArray);
}

/**
 * receive a message from a widget and pass to the GameEngine
 */
void GameEngine::UI::receiveMessage(UI_widget* from, uint32_t imessage)
{
	m_entity->sendMessage(imessage);
}

/**
 * (de)activate the timer
 */
void GameEngine::UI::timer(bool activate)
{
	if (activate) {
		g_gaWorld.registerTimerEvents(this->m_entity, true);
	}
	else {
		g_gaWorld.registerTimerEvents(this->m_entity, false);
	}
}

/**
 * receive and dispatch controller messages
 */
void GameEngine::UI::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::MOUSE_DOWN:
		onMouseDown(message);
		break;
	case gaMessage::Action::MOUSE_UP:
		onMouseUp(message);
		break;
	case gaMessage::Action::MOUSE_MOVE:
		onMouseMove(message);
		break;
	case gaMessage::Action::TIMER:
		onTimer(message);
		break;
	}
}

//------------------------------------------------------

/**
 *
 */
void GameEngine::UI_widget::sendMessage(UI_widget* from, uint32_t imessage)
{
	if (imessage == 0) {
		return;
	}

	if (m_parent) {
		m_parent->sendMessage(from, imessage);
	}
	else {
		m_ui->receiveMessage(from, imessage);
	}
}

GameEngine::UI_widget::UI_widget(const std::string& name, const glm::vec4& position, fwTexture* texture, bool visible) :
	GameEngine::Image2D(name, position, texture, &g_material),
	m_position_size(position)
{
	m_visible = visible;
	add_uniform("positionsize", m_position_size);
}

/**
 * add a widget on the panel
 */
void GameEngine::UI_widget::add(GameEngine::UI_widget* widget)
{
	widget->m_parent = this;
	addChild(widget);
}

/**
 * update screen position
 */
void GameEngine::UI_widget::update(const glm::vec4& parent)
{
	m_screen_position_size.w = m_position_size.w * parent.w;
	m_screen_position_size.z = m_position_size.z * parent.z;

	m_screen_position_size.x = parent.x + m_position_size.x * parent.z;
	m_screen_position_size.y = parent.y + m_position_size.y * parent.w;

	update();
}

/**
 * update children screen position
 */
void GameEngine::UI_widget::update(void)
{
	for (auto widget : m_children) {
		dynamic_cast<GameEngine::UI_widget*>(widget)->update(m_screen_position_size);
	}
}

/**
 * find relative widget
 */
GameEngine::UI_widget* GameEngine::UI_widget::findWidgetAt(const glm::vec2& cursor)
{
	if (!m_visible) {
		return nullptr;
	}

	// convert from parent coordinates to the widget corrdinates
	const glm::vec2 p = (cursor - m_gtranslation) / m_gscale ;

	if (p.x >= -1.0f && p.x <= 1.0f && p.y >= -1.0f && p.y <= 1.0f) {
		if (m_children.size() == 0) {
			return this;
		}

		GameEngine::UI_widget* w = nullptr;

		for (auto widget : m_children) {
			w = dynamic_cast<GameEngine::UI_widget*>(widget)->findWidgetAt(cursor);
			if (w != nullptr) {
				return w;
			}
		}
	}
	return nullptr;
}

/**
 * link each widget to the top UI
 */
void GameEngine::UI_widget::link(UI* ui)
{
	m_ui = ui;
	for (auto widget : m_children) {
		dynamic_cast<GameEngine::UI_widget*>(widget)->link(ui);
	}
}

/**
 * add buttons to the widget using a template
 */
void GameEngine::UI_widget::addButtons(std::vector<struct UI_def_button>& buttons)
{
	for (auto& button : buttons) {

		GameEngine::UI_button *b = new GameEngine::UI_button(
			button.name,
			button.position_size,
			button.img_release_position_size,
			button.img_press_position_size,
			button.texture,
			button.message,
			button.repeater
		);
		b->scale(button.scale);
		b->translate(button.translation);

		add(b);
	}
}

/**
 * get a specific widget
 */
UI_widget* GameEngine::UI_widget::widget(const std::string& name)
{
	for (auto widget : m_children) {
		if (widget->name() == name) {
			return dynamic_cast<UI_widget*>(widget);
		}
	}
	return nullptr;
}

/**
 * draw the GUI
 */
void GameEngine::UI_widget::draw(void)
{
	if (!m_visible) {
		return;
	}

	if (m_draw) {
		GameEngine::UI::draw_widget(
			m_screen_position_size
		);
	}

	for (auto widget : m_children) {
		dynamic_cast<GameEngine::UI_widget*>(widget)->draw();
	}
}

//-------------------------------------------------------

/**
 * send a message to the parent
 */
void GameEngine::UI_tab::sendMessage(UI_widget* from, uint32_t imessage)
{
	switch (imessage) {
	case GameEngine::UI_message::click:
		tab(dynamic_cast<UI_button*>(from));
		break;

	default:
		GameEngine::UI_widget::sendMessage(from, imessage);
	}
}

/**
 * create
 */
GameEngine::UI_tab::UI_tab(const std::string& name, const glm::vec4& position):
	UI_widget(name, position, nullptr)
{
	m_geometry = nullptr;	// virtual container, shall not be drawn
	m_material = nullptr;
}

/**
 * force the current tab
 */
void GameEngine::UI_tab::tab(UI_button* current)
{
	if (m_activeTab != nullptr) {
		dynamic_cast<UI_button*>(m_activeTab)->release();	// deactivate the previous button
		m_tabs[m_activeTab]->visible(false);
	}
	m_activeTab = current;
	m_activeTab->press();			// force the current button to 'press' image

	// clear the old tab
	//m_panel->clear();

	if (m_tabs[m_activeTab] != nullptr) {
		// and bind the the block to the display panel
		m_tabs[m_activeTab]->visible(true);
	}
}

/**
 * add a tab : button + pannel
 */
void GameEngine::UI_tab::addTab(UI_button* button, UI_widget* panel)
{
	m_tabs[button] = panel;

	panel->visible(false);

	// register the widget
	add(button);
	add(panel);
}

/**
 * define the display panel
 */
void GameEngine::UI_tab::setPanel(UI_widget* panel)
{
	m_panel = panel;
}

//-------------------------------------------------------

GameEngine::UI_picture::UI_picture(const std::string& name, const glm::vec4& panel, const glm::vec4& textureIndex, fwTexture* texture, bool visible):
	UI_widget(name, panel, texture, visible),
	m_textureIndex(textureIndex)
{
	m_draw = true;
	add_uniform("imagepos", m_textureIndex);
}

void GameEngine::UI_picture::draw(void)
{
	g_imagepos = m_textureIndex;
	UI_widget::draw();
}

// ------------------------------------------------------

GameEngine::UI_button::UI_button(const std::string& name, 
	const glm::vec4& position, 
	const glm::vec4& textureOffIndex, const glm::vec4& textureOnIndex, 
	fwTexture* texture,
	uint32_t message,
	bool repeater,
	bool visible
):
	UI_picture(name, position, textureOffIndex, texture, visible),
	m_texture_on(textureOnIndex),
	m_texture_off(textureOffIndex),
	m_message(message),
	m_repeater(repeater)
{
}

/**
 * activate the button
 */
void GameEngine::UI_button::onMouseDown(void)
{
	press();	// display the 'pressed' image
	if (m_repeater) {
		m_ui->timer(true);
	}
}

/**
 * reelase the button
 */
void GameEngine::UI_button::onMouseUp(void)
{
	release();	// display the 'released' image

	if (m_message == 0) {
		sendMessage(this, GameEngine::UI_message::click);
	}
	else {
		sendMessage(this, m_message);
	}
	if (m_repeater) {
		m_ui->timer(false);
	}
}

/**
 * force the OFF image
 */
void GameEngine::UI_button::release(void)
{
	m_textureIndex = m_texture_off;
}

/**
 *
 */
void GameEngine::UI_button::press(void)
{
	m_textureIndex = m_texture_on;
}

/**
 * cursor enters the button area
 */
void GameEngine::UI_button::onEnterArea(void)
{
	// display the 'pressed' image
	press();
	m_ui->timer(true);
}

/**
 * cursor leaves the button area
 */
void GameEngine::UI_button::onExitArea(void)
{
	// display the 'released' image
	release();
	m_ui->timer(false);
}

/**
 * if 'repeater' send a message while the button is pressed
 */
void GameEngine::UI_button::onTimer(void)
{
	if (m_message == 0) {
		sendMessage(this, GameEngine::UI_message::click);
	}
	else {
		sendMessage(this, m_message);
	}
}

// ------------------------------------------------------

GameEngine::UI_ZoomPicture::UI_ZoomPicture(const std::string& name, const glm::vec4& panel, const glm::vec4& textureIndex,
	const glm::ivec2& imageSize,
	const glm::ivec2& viewPort,
	fwTexture* texture,
	bool visible):
	UI_picture(name, panel, textureIndex, texture, visible),
	m_imageSize(imageSize),
	m_viewPort(viewPort),
	m_scroll(0, imageSize.y),
	m_size(textureIndex)
{
	m_draw = true;
	add_uniform("imagepos", m_textureIndex);

	m_textureIndex.w = m_viewPort.y * m_size.w / m_imageSize.y;
	m_textureIndex.y = m_size.y + m_scroll.y * m_size.w / m_imageSize.y - m_textureIndex.w;
}

void GameEngine::UI_ZoomPicture::draw(void)
{
	g_imagepos = m_textureIndex;
	g_imagepos.w = m_viewPort.y * m_textureIndex.w / m_imageSize.y;
	g_imagepos.y = m_textureIndex.y + m_scroll.y * m_textureIndex.w / m_imageSize.y - g_imagepos.w ;
	UI_widget::draw();
}

void GameEngine::UI_ZoomPicture::scrollUp(void)
{
	if (m_scroll.y < m_imageSize.y) {
		m_scroll.y++;
		m_textureIndex.w = m_viewPort.y * m_size.w / m_imageSize.y;
		m_textureIndex.y = m_size.y + m_scroll.y * m_size.w / m_imageSize.y - m_textureIndex.w;
	}
}

void GameEngine::UI_ZoomPicture::scrollDown(void)
{
	if (m_scroll.y > m_viewPort.y) {
		m_scroll.y--;
		m_textureIndex.w = m_viewPort.y * m_size.w / m_imageSize.y;
		m_textureIndex.y = m_size.y + m_scroll.y * m_size.w / m_imageSize.y - m_textureIndex.w;
	}
}

// ------------------------------------------------------

GameEngine::UI_container::UI_container(const std::string& name, const glm::vec4& panel, bool visible):
	UI_widget(name, panel, nullptr, visible)
{
	m_geometry = nullptr;	// virtual container, shall not be drawn
	m_material = nullptr;
}