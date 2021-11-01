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
 * place the weapon on screen
 */
void DarkForces::Actor::placeWeapon(DarkForces::Weapon::Kind weapon,
	const glm::vec2& delta)
{
	fwTexture* texture = g_hud[weapon];
	const DarkForces::Weapon* hud = m_weapon.get(weapon);

	int w, h, ch;
	texture->get_info(&w, &h, &ch);

	// darkforces draws in 320x200 but divide by 2
	float width = 2.0f * w / 640.0f;
	float height = 2.0f * h / 400.0f;

	g_dfHUD->setWeapon(texture, hud->HUDposition.x + delta.x, hud->HUDposition.y + delta.y, width, height);
}

/**
 * Change the current weapon
 */
void DarkForces::Actor::setWeapon(DarkForces::Weapon::Kind weapon)
{
	m_currentWeapon = weapon;

	const DarkForces::Weapon* hud = m_weapon.set(weapon);

	if (hud && g_hud.count(weapon) == 0) {
		dfBitmap* bmp = new dfBitmap(g_dfFiles, hud->HUDfile, static_cast<dfLevel*>(m_level)->palette());
		g_hud[weapon] = bmp->fwtexture();
	}

	// compute the size of the texture in glspace
	placeWeapon(weapon, glm::vec2(0, 0));
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
	if (m_currentWeapon == weapon) {
		return;
	}

	setWeapon(weapon);
}

/**
 * when the player moves
 */
void DarkForces::Actor::onMove(gaMessage* message)
{
	// detect when we start moving
	if (message->m_frame > m_frameStartMove + 2) {
		m_inMove = true;
		m_wobblingT = 0.0f;
		m_wobblingDirection = +0.0872665f;
	}
	m_frameStartMove = message->m_frame;

	m_wobbling.x = sin(m_wobblingT)/8.0f;
	m_wobbling.y = cos(m_wobblingT)/8.0f;

	m_wobblingT += m_wobblingDirection;

	if (m_wobblingT > 0.959931f) {
		m_wobblingDirection = -0.0872665f;
	}
	else if (m_wobblingT < -0.959931f) {
		m_wobblingDirection = +0.0872665f;
	}
	placeWeapon(m_currentWeapon, glm::vec2(m_wobbling.x, m_wobbling.y - m_wobbling.z));
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
		break;

	case gaMessage::Action::MOVE:
		onMove(message);
		break;

	case gaMessage::Action::LOOK_AT:
		// show more or less of the weapon based on the player look
		float y = (*(glm::vec3*)message->m_extra).y;
		const DarkForces::Weapon* hud = m_weapon.get(m_currentWeapon);

		float y1 = hud->HUDposition.y + m_wobbling.y - y;

		if (y1 > -0.51f) {
			m_wobbling.z = 0.51f + hud->HUDposition.y + m_wobbling.y;
		}
		else if (y1 < -1.1) {
			m_wobbling.z = 1.1f + hud->HUDposition.y + m_wobbling.y;
		}
		else {
			m_wobbling.z = y;
		}
		placeWeapon(m_currentWeapon, glm::vec2(m_wobbling.x, m_wobbling.y - m_wobbling.z));
		break;
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