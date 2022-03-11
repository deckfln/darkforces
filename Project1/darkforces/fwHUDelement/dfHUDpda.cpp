#include "dfHUDpda.h"

#include "../../framework/fwTexture.h"
#include "../../framework/fwTextureAtlas.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaComponent/gaCInventory.h"

#include "../dfPlayer.h"
#include "../dfFileLFD.h"

/**
 * quads to draw guns on the PDA
 */
static float gunsQuadVertices[] = { // vertex attributes for quads that fills the entire screen in Normalized Device Coordinates.
	// 1st column
	 // 1st row
	-1.0f,  1.0f,
	-1.0f,  0.6f,
	 0.0f,  0.6f,

	-1.0f,  1.0f,
	 0.0f,  0.6f,
	 0.0f,  1.0f,

	 // 2nd row
	-1.0f,  0.6f,
	-1.0f,  0.2f,
	 0.0f,  0.2f,

	-1.0f,  0.6f,
	 0.0f,  0.2f,
	 0.0f,  0.6f,

	 // 3rd row
	-1.0f,  0.2f,
	-1.0f, -0.2f,
	 0.0f, -0.2f,

	-1.0f,  0.2f,
	 0.0f, -0.2f,
	 0.0f,  0.2f,

	 // 4th row
	-1.0f, -0.2f,
	-1.0f, -0.6f,
	 0.0f, -0.6f,

	-1.0f, -0.2f,
	 0.0f, -0.6f,
	 0.0f, -0.2f,

	 // 5th row
	-1.0f, -0.6f,
	-1.0f, -1.0f,
	 0.0f, -1.0f,

	-1.0f, -0.6f,
	 0.0f, -1.0f,
	 0.0f, -0.6f,

	 // 2nd column
	  // 1st row
	  0.0f,  1.0f,
	  0.0f,  0.6f,
	  1.0f,  0.6f,

	  0.0f,  1.0f,
	  1.0f,  0.6f,
	  1.0f,  1.0f,

	  // 2nd row
	  0.0f,  0.6f,
	  0.0f,  0.2f,
	  1.0f,  0.2f,

	  0.0f,  0.6f,
	  1.0f,  0.2f,
	  1.0f,  0.6f,

	  // 3rd row
	  0.0f,  0.2f,
	  0.0f, -0.2f,
	  1.0f, -0.2f,

	  0.0f,  0.2f,
	  1.0f, -0.2f,
	  1.0f,  0.2f,

	  // 4th row
	  0.0f, -0.2f,
	  0.0f, -0.6f,
	  1.0f, -0.6f,

	  0.0f, -0.2f,
	  1.0f, -0.6f,
	  1.0f, -0.2f,

	  // 5th row
	  0.0f, -0.6f,
	  0.0f, -1.0f,
	  1.0f, -1.0f,

	  0.0f, -0.6f,
	  1.0f, -1.0f,
	  1.0f, -0.6f,
};

static float gunsQuadUvs[12 * 10]; // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.

static fwGeometry* g_geometry = nullptr;
static glVertexArray* g_vertexArray = nullptr;
static glProgram* g_program = nullptr;
static fwMaterial* g_material = nullptr;
static glm::vec4 g_screen;
static std::map<ShaderType, std::string> g_pdaShaders = {
	{VERTEX_SHADER, "darkforces/shaders/hud/pda_vs.glsl"},
	{FRAGMENT_SHADER, "darkforces/shaders/hud/pda_fs.glsl"}
};

static DarkForces::ANIM* g_pda = nullptr;
static DarkForces::ANIM* g_guns = nullptr;
static DarkForces::ANIM* g_items = nullptr;
static DarkForces::DELT* g_pda_background = nullptr;
static DarkForces::HUDelement::PDA* g_hud = nullptr;
static Framework::TextureAtlas* g_items_textures = nullptr;
static fwTexture* g_items_texture = nullptr;

/**
 *
 */
DarkForces::HUDelement::PDA::PDA(const std::string& name, Position position, fwHUDelementSizeLock lock, float width, float height):
	fwHUDelement(name, position, lock, width, height, nullptr, nullptr)
{
	if (g_pda == nullptr) {
		// preload the PDA background
		g_pda = DarkForces::FileLFD::loadAnim("pda", "MENU");

		g_items_textures = new Framework::TextureAtlas();

		// load all guns into a texturearray
		g_guns = DarkForces::FileLFD::loadAnim("guns", "DFBRIEF");
		size_t nbItems = g_guns->size();
		for (size_t i = 0; i < nbItems; i++) {
			g_items_textures->add(g_guns->texture(i)->texture());
		}

		// load all items into a texturearray
		g_items = DarkForces::FileLFD::loadAnim("items", "DFBRIEF");
		nbItems = g_items->size();
		for (size_t i = 0; i < nbItems; i++) {
			g_items_textures->add(g_items->texture(i)->texture());
		}

		g_items_texture = g_items_textures->generate();

		g_pda_background = g_pda->texture(0);
		m_texture = g_pda_background->texture();
	}

	if (g_geometry == nullptr) {
		g_material = new fwMaterial(g_pdaShaders);
		g_material->addTexture("image", (glTexture*)nullptr);
		g_material->addUniform(new fwUniform("onscreen", &g_screen));

		static glm::vec4 v(-0.08, 0.11, 0.0, 0.75);	// position of the list of guns on the pda
		g_material->set("onscreen", &v);
		g_material->set("image", g_items_texture);

		std::string vs = g_material->load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
		std::string fs = g_material->load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
		std::string gs = g_material->load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

		//preload the textureatlas texture position
		glm::vec4 tex;
		uint32_t i = 0, j=0;
		g_items_textures->texel(9, tex);	// display the empty slot
		for (size_t c = 0; c < g_guns->size(); c++) {
			gunsQuadUvs[j++] = tex.x;	gunsQuadUvs[j++] = tex.y + tex.w;
			gunsQuadUvs[j++] = tex.x;	gunsQuadUvs[j++] = tex.y;
			gunsQuadUvs[j++] = tex.x + tex.z;	gunsQuadUvs[j++] = tex.y;

			gunsQuadUvs[j++] = tex.x;			gunsQuadUvs[j++] = tex.y + tex.w;
			gunsQuadUvs[j++] = tex.x + tex.z;	gunsQuadUvs[j++] = tex.y;
			gunsQuadUvs[j++] = tex.x + tex.z;	gunsQuadUvs[j++] = tex.y + tex.w;

		}

		// create the program
		g_program = new glProgram(vs, fs, gs, "");

		g_geometry = new fwGeometry();
		g_geometry->addVertices("aPos", gunsQuadVertices, 2, sizeof(gunsQuadVertices), sizeof(float), false);
		g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, gunsQuadUvs, 2, sizeof(gunsQuadUvs), sizeof(float), false);

		g_vertexArray = new glVertexArray();
		g_geometry->enable_attributes(g_program);
		g_vertexArray->unbind();
	}
}

/**
 * // display the gun on the PDA
 */
void DarkForces::HUDelement::PDA::activateGun(uint32_t gunID)
{
	// draw the existing weapons
	glm::vec4 tex;
	uint32_t i = 0, j = 0;

	j = gunID * 12;

	g_items_textures->texel(gunID, tex);
	gunsQuadUvs[j++] = tex.x;	gunsQuadUvs[j++] = tex.y + tex.w;
	gunsQuadUvs[j++] = tex.x;	gunsQuadUvs[j++] = tex.y;
	gunsQuadUvs[j++] = tex.x + tex.z;	gunsQuadUvs[j++] = tex.y;

	gunsQuadUvs[j++] = tex.x;			gunsQuadUvs[j++] = tex.y + tex.w;
	gunsQuadUvs[j++] = tex.x + tex.z;	gunsQuadUvs[j++] = tex.y;
	gunsQuadUvs[j++] = tex.x + tex.z;	gunsQuadUvs[j++] = tex.y + tex.w;
	g_geometry->dirty();
}

void DarkForces::HUDelement::PDA::draw(fwFlatPanel* panel)
{
	if (!m_visible) {
		return;
	}

	fwHUDelement::draw(panel);	// draw the backgroup

	g_program->run();
	g_material->set_uniforms(g_program);
	g_geometry->draw(GL_TRIANGLES, g_vertexArray);
}
