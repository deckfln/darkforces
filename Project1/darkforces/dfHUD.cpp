#include "dfHUD.h"

#include "../framework/fwFlatPanel.h"
#include "../gaEngine/World.h"
#include "../gaEngine/gaComponent/gaController.h"
#include "dfFileSystem.h"
#include "dfLevel.h"
#include "dfFNT.h"
#include "dfFileLFD.h"
#include "gaItem/dfItem/dfWeapon.h"

DarkForces::HUD* g_dfHUD;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "darkforces/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "darkforces/shaders/hud/hud_fs.glsl"}
};

static glm::vec4 g_weaponTranform;

DarkForces::HUD::HUD(GameEngine::Level* level, fwScene* scene) :
	fwHUD("DarkForces::HUD", &g_subShaders)
{
	g_dfHUD = this;

	float w1, h1;

	// ammo display using default image2D
	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	dfBitmapImage* image = m_ammo_bmp->getImage(0);
	w1 = image->m_width / 320.0f;
	h1 = image->m_height / 200.0f;
	m_ammo = new GameEngine::Image2D(
		"darkforce:statusrt", 
		glm::vec2(w1, h1),					// width
		glm::vec2(1.0f - w1, h1 - 1.0f),	// position
		m_ammo_bmp->fwtexture()
	);
	scene->addMesh2D(m_ammo);							// add the healthbar on the HUD

	// text display using default image2D
	int32_t h, w, ch;
	uint8_t* data = m_text_bmp.get_info(&h, &w, &ch);
	if (data == nullptr) {
		m_text_bmp.data(new uint8_t[h * w * ch]);
		m_text_bmp.clear();
	}

	m_text = new GameEngine::Image2D(
		"text", 
		glm::vec2(1.0f, 0.05f),
		glm::vec2(0.0f, 0.95f),
		&m_text_bmp
	);
	scene->addMesh2D(m_text);							// add the healthbar on the HUD

	// prepare the entity part of the HUD
	m_compText.texture(&m_text_bmp);
	m_compText.ammo(m_ammo_bmp->fwtexture());

	m_entText.addComponent(&m_compText);

	m_entText.physical(false);
	m_entText.gravity(false);
	GameEngine::World::add(&m_entText);
}

/**
 * activate thre goggle green shader
 */
void DarkForces::HUD::setGoggle(bool onoff)
{
	m_material.b = onoff ? 1.0f : 0.0f;
	m_materialWeapon.b = m_material.b;
}

/**
 *
 */
void DarkForces::HUD::setScreenSize(float ratio)
{
	float w1, h1;
	m_ratio = ratio / 1.6f;

	// relocate the ammno
	dfBitmapImage* image = m_ammo_bmp->getImage(0);
	w1 = image->m_width / 320.0f / m_ratio;
	h1 = image->m_height / 200.0f;
	m_ammo->scale(glm::vec2(w1, h1));
	m_ammo->translate(glm::vec2(1.0f - w1, -1.0f + h1));
}

void DarkForces::HUD::setHeadlight(bool onoff)
{
	m_materialWeapon.g = onoff ? 1.0f : 0.0f;
}

DarkForces::HUD::~HUD()
{
	delete m_ammo;
	delete m_ammo_bmp;
	delete m_materialUniform;
}
