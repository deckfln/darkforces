#include "dfWeapon.h"

#include <imgui.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

#include "../../config.h"
#include "../dfMessage.h"
#include "../dfVOC.h"
#include "../dfBullet.h"
#include "../dfObject/dfSprite/dfSpriteAnimated/Enemy.h"

static std::map<DarkForces::Weapon::Kind, DarkForces::Weapon> g_WeaponSounds = {
	{DarkForces::Weapon::Kind::Concussion, {
		DarkForces::Weapon::Kind::Concussion,
		"CONCUSS5.VOC",
		100,
		0.1f,
		5,
		"concuss1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.0, 0.0)
		}
	},
	{DarkForces::Weapon::Kind::FusionCutter, {
		DarkForces::Weapon::Kind::FusionCutter,
		"FUSION1.VOC",
		100,
		0.1f,
		5,
		"fusion1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.0, 0.0)
		}
	},
	{DarkForces::Weapon::Kind::Missile, {
		DarkForces::Weapon::Kind::Missile,
		"MISSILE1.VOC",
		100,
		0.1f,
		5,
		"assault1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.0, 0.0)
		}
	},
	{DarkForces::Weapon::Kind::MortarGun, {
		DarkForces::Weapon::Kind::MortarGun,
		"MORTAR2.VOC",
		100,
		0.1f,
		5,
		"mortar1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.0, 0.0)
		}
	},
	{DarkForces::Weapon::Kind::Pistol, {
		DarkForces::Weapon::Kind::Pistol,
		"PISTOL-1.VOC",
		10,
		0.05f,
		1000,
		"pistol1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.3, -0.9)
		}
	},
	{DarkForces::Weapon::Kind::PlasmaCannon, {
		DarkForces::Weapon::Kind::PlasmaCannon,
		"PLASMA4.VOC",
		10,
		0.1f,
		5,
		"assault1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.0, 0.0)
		}
	},
	{DarkForces::Weapon::Kind::Repeater, {
		DarkForces::Weapon::Kind::Repeater,
		"REPEATER.VOC",
		10,
		0.1f,
		5,
		"autogun1",
		nullptr,
		nullptr,
		glm::vec2(0.2, 0.8)
		}
	},
	{DarkForces::Weapon::Kind::Rifle, {
		DarkForces::Weapon::Kind::Rifle,
		"RIFLE-1.VOC",
		15,
		0.2f,
		500,
		"rifle1.bm",
		nullptr,
		nullptr,
		glm::vec2(0.17, -0.83)
		}
	},
};

DarkForces::Component::Weapon::Weapon(void) :
	gaComponent(DF_COMPONENT_WEAPON)
{
}

/**
 *
 */
DarkForces::Component::Weapon::Weapon(DarkForces::Weapon::Kind weapon):
	gaComponent(DF_COMPONENT_WEAPON),
	m_kind(weapon)
{
	// prepare the sound component if there is a sound
	if (g_WeaponSounds.count(m_kind) > 0) {
		DarkForces::Weapon& w = g_WeaponSounds.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Enemy::Enemy::Sound::FIRE, loadVOC(w.m_fireSound)->sound());
	}
}

const DarkForces::Weapon* DarkForces::Component::Weapon::set(DarkForces::Weapon::Kind k)
{
	m_kind = k;
	if (g_WeaponSounds.count(m_kind) > 0) {
		DarkForces::Weapon& w = g_WeaponSounds.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Enemy::Enemy::Sound::FIRE, loadVOC(w.m_fireSound)->sound());

		return &w;
	}

	return nullptr;
}

const DarkForces::Weapon* DarkForces::Component::Weapon::get(DarkForces::Weapon::Kind k)
{
	m_kind = k;
	if (g_WeaponSounds.count(m_kind) > 0) {
		DarkForces::Weapon& w = g_WeaponSounds.at(m_kind);
		return &w;
	}

	return nullptr;
}

/**
 * fire a bullet
 */
void DarkForces::Component::Weapon::onFire(const glm::vec3& direction, time_t time)
{
	const DarkForces::Weapon& w = g_WeaponSounds.at(m_kind);

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
