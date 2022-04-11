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

	// health display using default image2D
	m_health_bmp = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(level)->palette());
	dfBitmapImage *image = m_health_bmp->getImage(0);
	w1 = image->m_width / 320.0f;
	h1 = image->m_height / 200.0f;
	m_health = new GameEngine::Image2D(
		"darkforce:statuslt", 
		glm::vec2(w1, h1),					// width
		glm::vec2(w1 - 1.0f, h1 - 1.0f),	// position
		m_health_bmp->fwtexture()
	);
	scene->addMesh2D(m_health);							// add the healthbar on the HUD
	
	// ammo display using default image2D
	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	image = m_ammo_bmp->getImage(0);
	w1 = image->m_width / 320.0f;
	h1 = image->m_height / 200.0f;
	m_ammo = new GameEngine::Image2D(
		"darkforce:statusrt", 
		glm::vec2(w1, h1),					// width
		glm::vec2(1.0f - w1, h1 - 1.0f),	// position
		m_ammo_bmp->fwtexture()
	);
	scene->addMesh2D(m_ammo);							// add the healthbar on the HUD

	// weapon display using a dedicated material
	m_weaponMaterial = new fwMaterial(g_subShaders);
	m_weaponMaterial->addTexture("image", (glTexture*)nullptr);
	m_weaponMaterial->addUniform(new fwUniform("material", &m_materialWeapon));
	m_weaponMaterial->addUniform(new fwUniform("transformation", &g_weaponTranform));

	m_weapon = new GameEngine::Image2D(
		"darkforce:weapon",
		glm::vec2(0.4, 0.4),
		glm::vec2(-0.2f, 0.4f - 1.0f),	// position
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
		glm::vec2(1.0f, 0.05f),
		glm::vec2(0.0f, 0.95f),
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

/**
 * Put the weapon texture on screen
 */
void DarkForces::HUD::setWeapon(fwTexture* texture, float x, float y, float w, float h)
{
	m_weapon_texture = texture;

	int32_t h1, w1, ch1;
	m_weapon_texture->get_info(&h1, &w1, &ch1);

	w = w1 / 320.0f / m_ratio;
	h = h1 / 200.0f;

	m_weapon->scale(glm::vec2(w, h));
	m_weapon->translate(glm::vec2(0, -1.0 + h));
	m_weapon->add_uniform("image", m_weapon_texture);
}

void DarkForces::HUD::setWeapon(fwTexture* texture, DarkForces::Weapon* weapon, float deltax, float deltay)
{
	m_weapon_texture = texture;

	int32_t h1, w1, ch1;
	m_weapon_texture->get_info(&w1, &h1, &ch1);

	// 320x200 top-left corner of the texture
	const glm::vec2& p = weapon->m_screenPosition[0];

	float h, w;
	w = w1 / 320.0f / m_ratio;
	h = h1 / 200.0f;

	float px = (p.x + w1 / 2.0f - 160.0) / 320.0f / m_ratio;
	float py = -(p.y + deltay + h1 / 2.0f) / 200.0f;

	// when looking down, limit the weapon sprite at the bottom of the sprite
	if (py > -1.0f + h1 / 200.0f) {
		py = -1.0f + h1 / 200.0f;
	}

	// when looking up, limit the weapon sprite at the top of the sprite
	if (py < -1.0f - h1 / 200.0f) {
		py = -1.0f - h1 / 200.0f;
	}

	m_weapon->scale(glm::vec2(w, h));
	m_weapon->translate(glm::vec2(px, py));
	m_weapon->add_uniform("image", m_weapon_texture);
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
 * change the weapon texture colo rbased on the current sector lightning
 */
void DarkForces::HUD::setAmbient(float ambient)
{
	m_materialWeapon.r = ambient;
}

/**
 *
 */
void DarkForces::HUD::setScreenSize(float ratio)
{
	float w1, h1;
	m_ratio = ratio / 1.6f;

	// relocate the health
	dfBitmapImage* image = m_health_bmp->getImage(0);
	w1 = image->m_width / 320.0f / m_ratio;
	h1 = image->m_height / 200.0f;
	m_health->scale(glm::vec2(w1, h1));
	m_health->translate(glm::vec2(-1.0f + w1, -1.0f + h1));

	// relocate the ammno
	image = m_ammo_bmp->getImage(0);
	w1 = image->m_width / 320.0f / m_ratio;
	h1 = image->m_height / 200.0f;
	m_ammo->scale(glm::vec2(w1, h1));
	m_ammo->translate(glm::vec2(1.0f - w1, -1.0f + h1));

	// relocate the weapon
	int32_t h, w, ch;
	m_weapon_texture->get_info(&h, &w, &ch);
	w1 = w / 320.0f / m_ratio;
	h1 = h / 200.0f;
	//m_weapon->scale(glm::vec2(w1, h1));
	//m_weapon->translate(glm::vec2(0, -1.0 + h1));
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
