#include "dfHUD.h"

#include "../framework/fwFlatPanel.h"
#include "../gaEngine/World.h"
#include "../gaEngine/gaComponent/gaController.h"
#include "dfFileSystem.h"
#include "dfLevel.h"
#include "dfFNT.h"
#include "dfFileLFD.h"

DarkForces::HUD* g_dfHUD;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "darkforces/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "darkforces/shaders/hud/hud_fs.glsl"}
};

DarkForces::HUD::HUD(GameEngine::Level* level, fwScene* scene) :
	fwHUD("DarkForces::HUD", &g_subShaders)
{
	g_dfHUD = this;

	// health display using default image2D
	m_health_bmp = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(level)->palette());
	m_health = new GameEngine::Image2D(
		"darkforce:statuslt", 
		0.2f, 0.2f,					// width
		0.2f - 1.0f, 0.2f - 1.0f,	// position
		m_health_bmp->fwtexture()
	);
	scene->addMesh2D(m_health);							// add the healthbar on the HUD
	
	// ammo display using default image2D
	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	m_ammo = new GameEngine::Image2D(
		"darkforce:statusrt", 
		0.2f, 0.2f,					// width
		1.0f - 0.2f, 0.2f - 1.0f,	// position
		m_ammo_bmp->fwtexture()
	);
	scene->addMesh2D(m_ammo);							// add the healthbar on the HUD

	// weapon display using a dedicated material
	m_weaponMaterial = new fwMaterial(g_subShaders);
	m_weaponMaterial->addTexture("image", (glTexture*)nullptr);
	m_weaponMaterial->addUniform(new fwUniform("onscreen", &m_weaponImage));
	m_weaponMaterial->addUniform(new fwUniform("material", &m_materialWeapon));

	m_weapon = new GameEngine::Image2D(
		"darkforce:weapon",
		0.4f, 0.4f, 
		-0.2f, 0.4f - 1.0f,	// position
		nullptr,
		m_weaponMaterial
	);
	scene->addMesh2D(m_weapon);							// add the healthbar on the HUD

	// text display using default image2D
	int32_t h, w, ch;
	uint8_t* data = m_text_bmp.get_info(&h, &w, &ch);
	if (data == nullptr) {
		m_text_bmp.data(new uint8_t[h * w * ch]);
		m_text_bmp.clear();
	}

	m_text = new GameEngine::Image2D(
		"text", 
		1.0f, 0.05f, 
		0.0f, 0.95f,
		&m_text_bmp
	);
	scene->addMesh2D(m_text);							// add the healthbar on the HUD

	// prepare the entity part of the HUD
	m_compText.texture(&m_text_bmp);
	m_compText.ammo(m_ammo_bmp->fwtexture());
	m_compText.shield(m_health_bmp->fwtexture());

	m_entText.addComponent(&m_compText);

	m_entText.physical(false);
	m_entText.gravity(false);
	GameEngine::World::add(&m_entText);
}

void DarkForces::HUD::setWeapon(fwTexture* texture, float x, float y, float w, float h)
{
	m_weaponImage = glm::vec4(w, h, x, y);

	m_weapon->add_uniform("image", texture);
	m_weapon->add_uniform("onscreen", m_weaponImage);
}

void DarkForces::HUD::setGoggle(bool onoff)
{
	m_material.b = onoff ? 1.0f : 0.0f;
	m_materialWeapon.b = m_material.b;
}

void DarkForces::HUD::setAmbient(float ambient)
{
	m_materialWeapon.r = ambient;
}

void DarkForces::HUD::setHeadlight(bool onoff)
{
	m_materialWeapon.g = onoff ? 1.0f : 0.0f;
}

DarkForces::HUD::~HUD()
{
	delete m_health;
	delete m_ammo;
	delete m_weapon;
	delete m_ammo_bmp;
	delete m_health_bmp;
	//delete m_panel;
	delete m_weaponMaterial;
	delete m_materialUniform;
	//delete m_uniWeapon;
}
