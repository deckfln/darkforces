 #include "dfCWeapon.h"

#include <imgui.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

#include "../../config.h"
#include "../dfMessage.h"
#include "../dfVOC.h"
#include "../dfBullet.h"
#include "../dfObject/Enemy.h"
#include "../dfComponent.h"

static std::map<DarkForces::Weapon::Kind, DarkForces::Weapon*> g_Weapons = {
	{DarkForces::Weapon::Kind::Concussion, &g_concussion},
	{DarkForces::Weapon::Kind::FusionCutter, &g_FusionCutter},
	{DarkForces::Weapon::Kind::Missile, &g_Missile},
	{DarkForces::Weapon::Kind::MortarGun, &g_MortarGun},
	{DarkForces::Weapon::Kind::Pistol, &g_Pistol},
	{DarkForces::Weapon::Kind::PlasmaCannon, &g_PlasmaCannon},
	{DarkForces::Weapon::Kind::Repeater, &g_Repeater},
	{DarkForces::Weapon::Kind::Rifle, &g_Rifle},
};

/**
 * define image position on screen
 */
void DarkForces::Component::Weapon::setImage(void)
{
	// change the texture of the Imagde2D
	fwTexture* texture = m_current->getStillTexture(static_cast<dfLevel*>(g_gaLevel)->palette());

	int32_t h1, w1, ch1;
	texture->get_info(&w1, &h1, &ch1);

	// 320x200 top-left corner of the texture
	const glm::vec2& p = m_current->m_screenPosition[0];

	const float ratio = m_ratio / 1.6f;
	const float w = w1 / 320.0f / ratio;
	const float h = h1 / 200.0f;

	const float px = (p.x + (float)w1 / 2.0f - 160.0f) / 320.0f / ratio + m_wobbling.x;
	float py = -(p.y + m_lookDownUp + h1 / 2.0f) / 200.0f + m_wobbling.y;

	// when looking down, limit the weapon sprite at the bottom of the sprite
	if (py > -1.0f + h1 / 200.0f) {
		py = -1.0f + h1 / 200.0f;
	}

	// when looking up, limit the weapon sprite at the top of the sprite
	if (py < -1.0f - h1 / 200.0f) {
		py = -1.0f - h1 / 200.0f;
	}

	m_image->scale(glm::vec2(w, h));
	m_image->translate(glm::vec2(px, py));
}

/**
 *
 */
DarkForces::Component::Weapon::Weapon(void) :
	gaComponent(DF_COMPONENT_WEAPON)
{
}

DarkForces::Component::Weapon::Weapon(DarkForces::Weapon* current) :
	gaComponent(DF_COMPONENT_WEAPON),
	m_current(current),
	m_kind(current->kind())
{
	// prepare the sound component if there is a sound
	if (g_Weapons.count(m_kind) > 0) {
		DarkForces::Weapon* w = g_Weapons.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Component::Actor::Sound::FIRE, loadVOC(w->m_fireSound)->sound());
	}
}

/**
 *
 */
DarkForces::Weapon* DarkForces::Component::Weapon::set(DarkForces::Weapon* current)
{
	m_current = current;
	m_kind = current->m_kind;
	if (g_Weapons.count(m_kind) > 0) {
		if (m_image != nullptr) {
			// add the sound of the weapon to the sound component
			m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Component::Actor::Sound::FIRE, loadVOC(current->m_fireSound)->sound());

			// change the texture of the Imagde2D
			fwTexture* texture = current->getStillTexture(static_cast<dfLevel*>(g_gaLevel)->palette());
			m_image->add_uniform("image", texture);

			setImage();
		}

		return current;
	}

	return nullptr;
}

/**
 *
 */
DarkForces::Weapon* DarkForces::Component::Weapon::get(void)
{
	return m_current;
}

/**
 * change the current weapon
 */
void DarkForces::Component::Weapon::onChangeWeapon(gaMessage* message)
{
	set(static_cast<DarkForces::Weapon*>(message->m_extra));
}

/**
 * fire a bullet
 */
void DarkForces::Component::Weapon::onFire(const glm::vec3& direction, time_t time)
{
	uint32_t nbAmmo = m_current->energy();
	// is there energy available for the weapon
	if (nbAmmo == 0) {
		return;
	}

	// count time since the last fire, and auto-fire at the weapon rate
	if (time - m_time < m_current->m_rate) {
		return;
	}
	m_time = time;

	nbAmmo = m_current->decreaseEnergy();
	m_entity->sendMessage(DarkForces::Message::AMMO, nbAmmo);

	// create a bullet based on the kind of weapon
	// and add to the world to live its life
	glm::vec3 p = m_entity->position();

	// apply a recoil effect
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right;
	glm::vec3 up;

	if (direction != glm::vec3(0)) {
		right = glm::normalize(glm::cross(_up, direction));
		up = glm::cross(direction, right);
	}
	else {
		up = _up;
	}

	p.y += m_entity->height() * m_ActorPosition.y;
	p += right * m_entity->radius() * m_ActorPosition.x;

	float x = ((float)rand()) / (float)RAND_MAX - 0.5f;
	float y = ((float)rand()) / (float)RAND_MAX - 0.5f;
	glm::vec3 d = direction + up * x * m_current->m_recoil + right * y * m_current->m_recoil;

	dfBullet* bullet = new dfBullet(m_current->m_damage, p + direction * m_entity->radius() * 2.0f, d);
	bullet->shooter(m_entity);

	GameEngine::World::add(bullet);

	m_entity->sendMessage(gaMessage::Action::PLAY_SOUND, DarkForces::Component::Actor::Sound::FIRE);
	m_entity->sendMessage(DarkForces::Message::FIRE);
}

/**
 * keep the finger on the trigger
 */
void DarkForces::Component::Weapon::onStopFire(gaMessage* message)
{
//	m_time = 0;
}

/**
 * when the player enter a new sector light
 */
void DarkForces::Component::Weapon::onChangeLightning(gaMessage* message)
{
	if (m_image) {
		m_material.r = message->m_fvalue;
	}
}

/**
 * when the screen gets resized
 */
void DarkForces::Component::Weapon::onScreenResize(gaMessage* message)
{
	if (m_image) {
		m_ratio = message->m_fvalue;

		fwTexture* texture = m_current->getStillTexture(static_cast<dfLevel*>(g_gaLevel)->palette());

		// relocate the weapon
		int32_t h, w, ch;
		texture->get_info(&h, &w, &ch);

		float w1, h1;
		w1 = w / 320.0f / m_ratio;
		h1 = h / 200.0f;
		m_image->scale(glm::vec2(w1, h1));
		//m_weapon->translate(glm::vec2(0, -1.0 + h1));
	}
}

/**
 * when the player looks somewhere
 */
void DarkForces::Component::Weapon::onLookAt(gaMessage* message)
{
	if (m_image) {
		// show more or less of the weapon based on the player look
		// convert from opengl -1:+1, to vga 0:200
		m_lookDownUp = message->m_v3value.y * 100.0f;
		setImage();
	}
}

/**
 * when the player moves
 */
void DarkForces::Component::Weapon::onMove(gaMessage* message)
{
	if (m_image) {
			// detect when we start moving
		if (message->m_frame > m_frameStartMove + 2) {
			m_wobblingT = 0.0f;
			m_wobblingDirection = +0.0872665f;
		}
		m_frameStartMove = message->m_frame;

		m_wobbling.x = sin(m_wobblingT) / 8.0f;
		m_wobbling.y = cos(m_wobblingT) / 8.0f;

		printf("%f**\n", m_wobbling.y);

		m_wobblingT += m_wobblingDirection;

		if (m_wobblingT > 0.959931f) {
			m_wobblingDirection = -0.0872665f;
		}
		else if (m_wobblingT < -0.959931f) {
			m_wobblingDirection = +0.0872665f;
		}

		setImage();
	}
}

//---------------------------------------

/**
 * Increase or decrease the energy and update the HUD
 */
void DarkForces::Component::Weapon::addEnergy(int32_t value)
{
	uint32_t nbAmmo = m_current->addEnergy(value);
	m_entity->sendMessage(DarkForces::Message::AMMO, nbAmmo);
}


/**
 * build and return an Image2D for the HUD
 */
GameEngine::Image2D* DarkForces::Component::Weapon::getImage(void)
{
	// date for display
	static std::map<ShaderType, std::string> g_subShaders = {
		{VERTEX_SHADER, "darkforces/shaders/hud/hud_vs.glsl"},
		{FRAGMENT_SHADER, "darkforces/shaders/hud/hud_fs.glsl"}
	};

	static fwMaterial* g_weaponMaterial = nullptr;
	static glm::vec4 g_weaponTranform;

	if (g_weaponMaterial == nullptr) {
		// weapon display using a dedicated material
		g_weaponMaterial = new fwMaterial(g_subShaders);
		g_weaponMaterial->addTexture("image", (glTexture*)nullptr);
		g_weaponMaterial->addUniform(new fwUniform("material", &m_material));
		g_weaponMaterial->addUniform(new fwUniform("transformation", &g_weaponTranform));
	}

	m_image = new GameEngine::Image2D(
		"darkforce:weapon",
		glm::vec2(0.4, 0.4),
		glm::vec2(-0.2f, 0.4f - 1.0f),	// position
		nullptr,
		g_weaponMaterial
	);

	return m_image;
}

/**
 *
 */
void DarkForces::Component::Weapon::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::CHANGE_WEAPON:
		onChangeWeapon(message);
		break;

	case DarkForces::Message::START_FIRE:
		onFire(*(glm::vec3*)message->m_extra, message->m_time);
		break;

	case DarkForces::Message::STOP_FIRE:
		onStopFire(message);
		break;

	case DarkForces::Message::ADD_ENERGY:
		addEnergy(message->m_value);
		break;

	case DarkForces::Message::AMBIENT:
		onChangeLightning(message);
		break;

	case gaMessage::Action::SCREEN_RESIZE:
		onScreenResize(message);
		break;

	case gaMessage::Action::LOOK_AT:
		onLookAt(message);
		break;

	case gaMessage::Action::MOVE:
		onMove(message);
		break;
	}
}

//-----------------------------------

void DarkForces::Component::Weapon::debugGUIinline(void)
{
	if (ImGui::TreeNode("Weapon")) {
		ImGui::Text("Type: %s", g_Weapons[m_kind]->m_debug);
		ImGui::TreePop();
	}
}
