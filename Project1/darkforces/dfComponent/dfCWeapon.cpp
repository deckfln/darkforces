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
	if (g_Weapons.count(m_kind) > 0) {
		DarkForces::Weapon* w = g_Weapons.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Component::Actor::Sound::FIRE, loadVOC(w->m_fireSound)->sound());
	}
}

DarkForces::Weapon* DarkForces::Component::Weapon::set(DarkForces::Weapon::Kind k)
{
	m_kind = k;
	if (g_Weapons.count(m_kind) > 0) {
		DarkForces::Weapon* w = g_Weapons.at(m_kind);
		m_entity->sendMessage(gaMessage::Action::REGISTER_SOUND, DarkForces::Component::Actor::Sound::FIRE, loadVOC(w->m_fireSound)->sound());

		return w;
	}

	return nullptr;
}

DarkForces::Weapon* DarkForces::Component::Weapon::get(DarkForces::Weapon::Kind k)
{
	m_kind = k;
	if (g_Weapons.count(m_kind) > 0) {
		DarkForces::Weapon* w = g_Weapons.at(m_kind);
		return w;
	}

	return nullptr;
}

/**
 * change the current weapon
 */
void DarkForces::Component::Weapon::onChangeWeapon(gaMessage* message)
{
	set((DarkForces::Weapon::Kind)message->m_value);
}

/**
 * fire a bullet
 */
void DarkForces::Component::Weapon::onFire(const glm::vec3& direction, time_t time)
{
	const DarkForces::Weapon* w = g_Weapons.at(m_kind);

	// is there energy available for the weapon
	if (m_energy == 0) {
		return;
	}

	// count time since the last fire, and auto-fire at the weapon rate
	if (time - m_time < w->m_rate) {
		return;
	}
	m_time = time;

	m_energy--;

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
	glm::vec3 d = direction + up * x * w->m_recoil + right * y * w->m_recoil;

	dfBullet* bullet = new dfBullet(w->m_damage, p + direction * m_entity->radius() * 2.0f, d);
	bullet->shooter(m_entity);

	g_gaWorld.addClient(bullet);

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
 * Drop ammo or rifle when the entity is dying
 *  if the entity holds a rifle, drop it with the ammo
 *  if it holds a rifle, drop the ammo
 */
void DarkForces::Component::Weapon::onDying(gaMessage* message)
{
	switch (m_kind) {
	case DarkForces::Weapon::Kind::Rifle:
		m_entity->sendMessage(gaMessage::Action::DROP_ITEM, dfLogic::ITEM_RIFLE, m_energy);
		break;
	case DarkForces::Weapon::Kind::Pistol:
		m_entity->sendMessage(gaMessage::Action::DROP_ITEM, dfLogic::ITEM_ENERGY, m_energy);
		break;
	}
}

/**
 * Increase or decrease the energy and update the HUD
 */
void DarkForces::Component::Weapon::addEnergy(int32_t value)
{
	m_energy += value;
	if (m_energy > m_maxEnergy) {
		m_energy = m_maxEnergy;
	}
	else if (m_energy < 0) {
		m_energy = 0;
	}
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

	case DarkForces::Message::DYING:
		onDying(message);
		break;
	}
}

void DarkForces::Component::Weapon::debugGUIinline(void)
{
	if (ImGui::TreeNode("Weapon")) {
		ImGui::Text("Type: %s", g_Weapons[m_kind]->m_debug);
		ImGui::Text("Energy: %d", m_energy);
		ImGui::TreePop();
	}
}
