#include "gaUI.h"

#include <map>

#include "../glEngine/glUniformBuffer.h"

#include "../framework/fwHUDelement.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwUniform.h"
#include "../framework/fwMaterial.h"
#include "../framework/fwFlatPanel.h"

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

GameEngine::UI::UI(const std::string& name, Framework::TextureAtlas* textures, bool visible):
	fwHUD(name, &g_subShaders, visible),
	m_textures(textures)
{
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
	g_material->set_uniforms(g_program);
	g_geometry->draw(GL_TRIANGLES, g_vertexArray);
}

//------------------------------------------------------

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


GameEngine::UI_tab::UI_tab(const std::string& name, const glm::vec4& panel, const glm::vec4& button):
	UI_widget(name, button)
{
	add(new UI_widget("tab", panel));
}

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
