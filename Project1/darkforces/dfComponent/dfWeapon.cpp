#include "dfWeapon.h"

#include <imgui.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

#include "../../config.h"
#include "../dfMessage.h"
#include "../dfVOC.h"
#include "../dfBullet.h"
#include "../dfObject/dfSprite/dfSpriteAnimated/Enemy.h"

struct WeaponD {
	DarkForces::Component::Weapon::Kind m_kind;
	const char* m_fireSound;
	uint32_t m_damage;		// damage per bullet
	float m_recoil;			// bullet dispersion based on recoil strength
	time_t m_rate;			// how many bullets per seconds
};

static const std::map<DarkForces::Component::Weapon::Kind, WeaponD> g_WeaponSounds = {
	{DarkForces::Component::Weapon::Kind::Concussion, {
		DarkForces::Component::Weapon::Kind::Concussion,
		"CONCUSS5.VOC",
		100,
		0.1f,
		5}
	},
	{DarkForces::Component::Weapon::Kind::FusionCutter, {
		DarkForces::Component::Weapon::Kind::FusionCutter,
		"FUSION1.VOC",
		100,
		0.1f,
		5}
	},
	{DarkForces::Component::Weapon::Kind::Missile, {
		DarkForces::Component::Weapon::Kind::Missile,
		"MISSILE1.VOC",
		100,
		0.1f,
		5}
	},
	{DarkForces::Component::Weapon::Kind::MortarGun, {
		DarkForces::Component::Weapon::Kind::MortarGun,
		"MORTAR2.VOC",
		100,
		0.1f,
		5}
	},
	{DarkForces::Component::Weapon::Kind::Pistol, {
		DarkForces::Component::Weapon::Kind::Pistol,
		"PISTOL-1.VOC",
		10,
		0.05f,
		1000}
	},
	{DarkForces::Component::Weapon::Kind::PlasmaCannon, {
		DarkForces::Component::Weapon::Kind::PlasmaCannon,
		"PLASMA4.VOC",
		10,
		0.1f,
		5}
	},
	{DarkForces::Component::Weapon::Kind::Repeater, {
		DarkForces::Component::Weapon::Kind::Repeater,
		"REPEATER.VOC",
		10,
		0.1f,
		5}
	},
	{DarkForces::Component::Weapon::Kind::Rifle, {
		DarkForces::Component::Weapon::Kind::Rifle,
		"RIFLE-1.VOC",
		15,
		0.2f,
		500}
	},
};

DarkForces::Component::Weapon::Weapon(void) :
	gaComponent(DF_COMPONENT_WEAPON)
{
}

/**
 *
 */
DarkForces::Component::Weapon::Weapon(Kind weapon):
	gaComponent(DF_COMPONENT_WEAPON),
	m_kind(weapon)
{
	// prepare the sound component if there is a sound
	if (g_WeaponSounds.count(m_kind) > 0) {
		const WeaponD& w = g_WeaponSounds.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Enemy::Enemy::Sound::FIRE, loadVOC(w.m_fireSound)->sound());
	}
}

void DarkForces::Component::Weapon::set(Kind k)
{
	m_kind = k;
	if (g_WeaponSounds.count(m_kind) > 0) {
		const WeaponD& w = g_WeaponSounds.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Enemy::Enemy::Sound::FIRE, loadVOC(w.m_fireSound)->sound());
	}
}

/**
 * fire a bullet
 */
void DarkForces::Component::Weapon::onFire(const glm::vec3& direction, time_t time)
{
	const WeaponD& w = g_WeaponSounds.at(m_kind);

	// count time since the last fire, and auto-fire at the weapon rate
	if (time - m_time < w.m_rate) {
		return;
	}
	m_time = time;

	// create a bullet based on the kind of weapon
	// and add to the world to live its life
	glm::vec3 p = m_entity->position();
	p.y += m_entity->height() / 2.0f;

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

	float x = ((float)rand()) / (float)RAND_MAX - 0.5f;
	float y = ((float)rand()) / (float)RAND_MAX - 0.5f;
	glm::vec3 d = direction + up * x * w.m_recoil + right * y * w.m_recoil;

	dfBullet* bullet = new dfBullet(w.m_damage, p + d* m_entity->radius() * 2.0f, d);

	g_gaWorld.addClient(bullet);

	m_entity->sendMessage(gaMessage::Action::PLAY_SOUND, DarkForces::Enemy::Enemy::Sound::FIRE);
}

/**
 *
 */
void DarkForces::Component::Weapon::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::FIRE:
		onFire(*(glm::vec3*)message->m_extra, message->m_time);
		break;
	}
}

void DarkForces::Component::Weapon::debugGUIinline(void)
{
	if (ImGui::TreeNode("Weapon")) {
		ImGui::TreePop();
	}
}
