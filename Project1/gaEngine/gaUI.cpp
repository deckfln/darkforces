#include "gaUI.h"

#include <map>

#include "../glEngine/glUniformBuffer.h"

#include "../framework/fwHUDelement.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwUniform.h"
#include "../framework/fwMaterial.h"
#include "../framework/fwFlatPanel.h"

#include "gaEntity.h"

static fwFlatPanel* g_hudPanel = nullptr;
static fwMaterial* g_material = nullptr;
static fwUniform* g_uniformTexture = nullptr;
static glm::vec4 g_screen;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "gaEngine/shaders/gui_vs.glsl"},
	{FRAGMENT_SHADER, "gaEngine/shaders/gui_fs.glsl"}
};

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

//-------------------------------------------------------

/**
 * dipatch mouse actions to widgets
 */
void GameEngine::UI::onMouseDown(gaMessage* message)
{
	float x = message->m_v3value.x;
	float y = message->m_v3value.y;
	uint32_t button = message->m_value;

	// find the widget that is under the cursor
	m_currentButton = m_root->findWidgetAt(x, y);
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
 * create an UI
 */
GameEngine::UI::UI(const std::string& name, bool visible):
	fwHUD(name, &g_subShaders, visible),
	gaComponent(gaComponent::Gui)
{
}

/**
 * draw the GUI
 */
void GameEngine::UI::draw(void)
{
	if (!m_visible) {
		return;
	}

	if (g_material == nullptr) {
		g_material = new fwMaterial(g_subShaders);
		g_material->addTexture("image", m_textures->texture());
		g_material->addUniform(&g_uni_ps);
		g_material->addUniform(&g_uni_ip);

		std::string vs = g_material->load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
		std::string fs = g_material->load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
		std::string gs = g_material->load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

		g_program = new glProgram(vs, fs, gs, "");

		g_geometry = new fwGeometry();
		g_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);

		g_vertexArray = new glVertexArray();
		g_geometry->enable_attributes(g_program);
		g_vertexArray->unbind();
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
	g_material->set_uniforms(g_program);
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

GameEngine::UI_widget::UI_widget(const std::string& name, const glm::vec4& position, bool visible) :
	m_name(name),
	m_position_size(position),
	m_visible(visible)
{
}

/**
 * add a widget on the panel
 */
void GameEngine::UI_widget::add(GameEngine::UI_widget* widget)
{
	widget->m_parent = this;
	m_widgets.push_back(widget);
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

	for (auto widget : m_widgets) {
		widget->update(m_screen_position_size);
	}
}

/**
 * find relative widget
 */
GameEngine::UI_widget* GameEngine::UI_widget::findWidgetAt(float x, float y)
{
	if (!m_visible) {
		return nullptr;
	}

	if (x >= m_screen_position_size.x && x <= m_screen_position_size.x + m_screen_position_size.z &&
		y >= m_screen_position_size.y && y <= m_screen_position_size.y + m_screen_position_size.w
		) {
		if (m_widgets.size() == 0) {
			return this;
		}

		GameEngine::UI_widget* w = nullptr;

		for (auto widget : m_widgets) {
			w = widget->findWidgetAt(x, y);
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
	for (auto widget : m_widgets) {
		widget->link(ui);
	}
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

	for (auto widget : m_widgets) {
		widget->draw();
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
	UI_widget(name, position)
{
}

/**
 * force the current tab
 */
void GameEngine::UI_tab::tab(UI_button* current)
{
	if (m_activeTab != nullptr) {
		dynamic_cast<UI_button*>(m_activeTab)->release();	// deactivate the previous button

		if (m_tabs[m_activeTab] != nullptr) {
			// and hide the panel
			m_tabs[m_activeTab]->visible(false);
		}

	}
	m_activeTab = current;
	m_activeTab->press();			// force the current tab on

	if (m_tabs[m_activeTab] != nullptr) {
		// and display the panel
		m_tabs[m_activeTab]->visible(true);
	}
}

/**
 * add a tab : button + pannel
 */
void GameEngine::UI_tab::addTab(UI_button* button, UI_widget* panel)
{
	m_tabs[button] = panel;

	// register the widget
	add(button);
	if (panel) {
		add(panel);		
		panel->visible(false);	// hide the panel by default
	}
}

//-------------------------------------------------------

GameEngine::UI_picture::UI_picture(const std::string& name, const glm::vec4& panel, const glm::vec4& textureIndex, bool visible):
	UI_widget(name, panel, visible),
	m_textureIndex(textureIndex)
{
	m_draw = true;
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
	uint32_t message,
	bool visible):
	UI_picture(name, position, textureOffIndex, visible),
	m_texture_on(textureOnIndex),
	m_texture_off(textureOffIndex),
	m_message(message)
{
}

/**
 * activate the button
 */
void GameEngine::UI_button::onMouseDown(void)
{
	press();
}

/**
 * reelase the button
 */
void GameEngine::UI_button::onMouseUp(void)
{
	release();

	if (m_message == 0) {
		sendMessage(this, GameEngine::UI_message::click);
	}
	else {
		sendMessage(this, m_message);
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
