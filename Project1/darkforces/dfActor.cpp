#include "dfActor.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "dfFileSystem.h"
#include "dfSector.h"
#include "dfLevel.h"
#include "dfHUD.h"

static const char* g_className = "dfActor";

static std::map<DarkForces::Weapon::Kind, fwTexture*> g_hud;

static std::map<uint32_t, DarkForces::Weapon::Kind> g_WeaponKeys = {
	{GLFW_KEY_1, DarkForces::Weapon::Kind::Pistol},
	{GLFW_KEY_2, DarkForces::Weapon::Kind::Rifle}
};

DarkForces::Actor::Actor(int mclass, const std::string& name, fwCylinder& cylinder, const glm::vec3& feet, float eyes, float ankle) :
	gaActor(mclass, name, cylinder, feet, eyes, ankle)
{
	m_className = g_className;

	addComponent(&m_defaultAI);
	addComponent(&m_sound);
	addComponent(&m_weapon);
	m_weapon.set(DarkForces::Weapon::Kind::Rifle);
}

DarkForces::Actor::Actor(flightRecorder::Entity* record) :
	gaActor(record)
{
	m_className = g_className;
}

/**
 * bind the level
 */
void DarkForces::Actor::bind(dfLevel* level)
{
	m_level = level;
	m_defaultAI.bind(level);
}

/**
 * Change the current weapon
 */
void DarkForces::Actor::onChangeWeapon(int kweapon)
{
	if (g_WeaponKeys.count(kweapon) == 0) {
		return;
	}

	DarkForces::Weapon::Kind weapon = g_WeaponKeys[kweapon];
	setWeapon(weapon);
}

/**
 * Change the current weapon
 */
void DarkForces::Actor::setWeapon(DarkForces::Weapon::Kind weapon)
{
	const DarkForces::Weapon* hud = m_weapon.set(weapon);

	if (hud && g_hud.count(weapon) == 0) {
		dfBitmap* bmp = new dfBitmap(g_dfFiles, hud->HUDfile, static_cast<dfLevel*>(m_level)->palette());
		g_hud[weapon] = bmp->fwtexture();
	}

	float x = 0.5f;
	// compute the size of the texture in glspace
	fwTexture* texture = g_hud[weapon];
	int w, h, ch;
	texture->get_info(&w, &h, &ch);

	// darkforces draws in 320x200 but divide by 2
	float width = 2.0f * w / 640.0f;
	float height = 2.0f * h / 400.0f;

	g_dfHUD->setWeapon(g_hud[weapon], hud->HUDposition.x, hud->HUDposition.y, width, height);
}


/**
 * let an entity deal with a situation
 */
void DarkForces::Actor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case gaMessage::KEY:
		switch (message->m_value) {
		case GLFW_KEY_LEFT_CONTROL:
			sendMessage(DarkForces::Message::FIRE, 0, &m_direction);
			break;

		case GLFW_KEY_F5:
			if (!m_headlight) {
				m_headlight = true;
			}
			else {
				m_headlight = false;
			}
			break;

		default:
			if (g_WeaponKeys.count(message->m_value) > 0) {
				onChangeWeapon(message->m_value);
			}
		}
	}

	gaActor::dispatchMessage(message);
}

/**
 * return a record of the entity state (for debug)
 */
uint32_t DarkForces::Actor::recordState(void* record)
{
	flightRecorder::DarkForces::Actor* r = static_cast<flightRecorder::DarkForces::Actor*>(record);
	gaActor::recordState(&r->actor);
	r->actor.entity.size = sizeof(flightRecorder::DarkForces::Actor);
	r->actor.entity.classID = flightRecorder::TYPE::DF_ACTOR;

	if (m_defaultAI.currentSector() != nullptr) {
		strncpy_s(r->currentSector, m_defaultAI.currentSector()->name().c_str(), sizeof(r->currentSector));
	}
	else {
		r->currentSector[0] = 0;
	}

	return sizeof(flightRecorder::DarkForces::Actor);
}

/**
 * reload an actor state from a record
 */
void DarkForces::Actor::loadState(void* record)
{
	flightRecorder::DarkForces::Actor* r = (flightRecorder::DarkForces::Actor*)record;
	gaActor::loadState(&r->actor);
	if (r->currentSector[0] != 0) {
		m_defaultAI.currentSector(m_level->findSector(r->currentSector));
	}
	else {
		m_defaultAI.currentSector(nullptr);
	}
}

/**
 * Add dedicated component debug the entity
 */
void DarkForces::Actor::debugGUIChildClass(void)
{
	gaActor::debugGUIChildClass();

	if (ImGui::TreeNode("dfActor")) {
		ImGui::TreePop();
	}
}