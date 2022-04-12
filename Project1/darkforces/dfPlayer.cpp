#include "dfPlayer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../gaEngine/World.h"

#include "dfSounds.h"
#include "dfFileSystem.h"
#include "dfSector.h"
#include "dfLevel.h"
#include "dfHUD.h"
#include "dfVOC.h"
#include "gaItem/dfItem/dfHeadlight.h"

static const char* g_className = "dfPlayer";

namespace DarkForces {
	struct weaponTexture {
		fwTexture* m_still = nullptr;
		fwTexture* m_fire = nullptr;
	};
}

static std::map<DarkForces::Weapon::Kind, DarkForces::weaponTexture> g_hud;

static std::map<uint32_t, std::string> g_WeaponKeys = {
	{GLFW_KEY_1, "DarkForces:weapon:pistol"},
	{GLFW_KEY_2, "DarkForces:weapon:rifle"}
};

/**
 *
 */
DarkForces::Player::Player(int mclass, const std::string& name, fwCylinder& cylinder, const glm::vec3& feet, float eyes, float ankle) :
	gaActor(mclass, name, cylinder, feet, eyes, ankle)
{
	m_className = g_className;

	m_pistol.clone(&g_Pistol);
	m_rifle.clone(&g_Rifle);

	addComponent(&m_defaultAI);
	addComponent(&m_sound);
		m_sound.addSound(DarkForces::Sounds::PLAYER_HIT_BY_STORM_COMMANDO_OFFICER, DarkForces::loadSound(DarkForces::Sounds::PLAYER_HIT_BY_STORM_COMMANDO_OFFICER)->sound());
		m_sound.addSound(DarkForces::Sounds::PLAYER_NEARLY_HIT, DarkForces::loadSound(PLAYER_NEARLY_HIT)->sound());
	addComponent(&m_weapon);
	addComponent(&m_inventory);
		m_inventory.add(&m_headlight);
		m_inventory.add(&m_clip);
		m_inventory.add(&m_pistol);
			m_pistol.loadClip();
	addComponent(&m_automap);

	m_defaultAI.setClass("player");
	sendMessage("hud", DarkForces::Message::AMMO, m_clip.energy());
	sendMessage("pda", gaMessage::ADD_ITEM, 0, &m_pistol);
}

/**
 *
 */
DarkForces::Player::Player(flightRecorder::Entity* record) :
	gaActor(record)
{
	m_className = g_className;
}

/**
 * bind the level
 */
void DarkForces::Player::bind(dfLevel* level)
{
	m_level = level;
	m_defaultAI.bind(level);
}
/**
 * the item is present in the inventory and is turned on
 */
bool DarkForces::Player::isOn(const std::string& name)
{
	DarkForces::Headlight* item = dynamic_cast<DarkForces::Headlight * >(m_inventory.get(name));
	if (item == nullptr) {
		return false;
	}
	return item->on();
}

/**
 * add item to inventory
 */
void DarkForces::Player::addItem(Item* item)
{
	m_inventory.add(item);
}

/**
 * place the weapon on screen
 */
void DarkForces::Player::placeWeapon(DarkForces::Weapon* weapon,
	const glm::vec2& delta)
{
	fwTexture* texture;
	float fire_y;				// move the weapon up if the weapon is firing

	if (m_weaponFiring) {
		texture = g_hud[weapon->kind()].m_fire;
		fire_y = 0.1;
	}
	else {
		texture = g_hud[weapon->kind()].m_still;
		fire_y = 0;
	}

	//g_dfHUD->setWeapon(texture, weapon, delta.x, delta.y);
}

/**
 * Change the current weapon
 */
void DarkForces::Player::setWeapon(DarkForces::Weapon* weapon)
{
	m_currentWeapon = weapon;

	m_weapon.set(weapon);

	if (g_hud.count(weapon->kind()) == 0) {
		g_hud[weapon->kind()].m_still = weapon->getStillTexture(static_cast<dfLevel*>(m_level)->palette());
		g_hud[weapon->kind()].m_fire = weapon->getFireTexture(static_cast<dfLevel*>(m_level)->palette());
	}

	// compute the size of the texture in glspace
	placeWeapon(weapon, glm::vec2(0, 0));
}

void DarkForces::Player::setWeapon()
{
	setWeapon(&m_pistol);
}

/**
 * Change the current weapon
 */
void DarkForces::Player::onChangeWeapon(int kweapon)
{
	if (g_WeaponKeys.count(kweapon) == 0) {
		return;
	}

	const std::string name = g_WeaponKeys[kweapon];
	GameEngine::Item* weapon = m_inventory.get(name);
	if (weapon == nullptr) {
		// player doesn't have the weapong in its inventory
		return;
	}

	if (m_currentWeapon == weapon) {
		return;
	}
	setWeapon(dynamic_cast<DarkForces::Weapon*>(weapon));
}

/**
 * when the player moves
 */
void DarkForces::Player::onMove(gaMessage* message)
{
	/*
	// detect when we start moving
	if (message->m_frame > m_frameStartMove + 2) {
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
	*/

	// extract our sector and get the lighthing for the hud
	dfSector* sector = static_cast<dfLevel*>(g_gaLevel)->findSector(position());
	if (sector != nullptr) {
		float ambient = sector->ambient() / 32.0f;
		sendMessage(DarkForces::Message::AMBIENT, 0, ambient);
		//g_dfHUD->setAmbient(ambient);
	}
}

/**
 * when the player fires
 */
void DarkForces::Player::onFire(gaMessage* message)
{
	m_weaponFiring = true;
	placeWeapon(m_currentWeapon, glm::vec2(m_wobbling.x, m_wobbling.y - m_wobbling.z));

	// reset the fire position after 10 frames
	GameEngine::Alarm alarm(this, 10, gaMessage::Action::ALARM);
	g_gaWorld.registerAlarmEvent(alarm);
}

/**
 * time to display the fire texture is over
 */
void DarkForces::Player::onAlarm(gaMessage* message)
{
	m_weaponFiring = false;
	placeWeapon(m_currentWeapon, glm::vec2(m_wobbling.x, m_wobbling.y - m_wobbling.z));
}

/**
 * when the player looks somewhere
 *
void DarkForces::Player::onLookAt(gaMessage* message)
{
	// show more or less of the weapon based on the player look
	float y = message->m_v3value.y;

	// convert from opengl -1:+1, to vga 0:200
	float dfy = y * 100.0f;

	float y1 = (1.0f - m_currentWeapon->m_screenPosition[0].y / 100.0f) + m_wobbling.y + y;

	if (y1 > -0.51f) {
		m_wobbling.z = 0.51f + m_wobbling.y;
	}
	else if (y1 < -1.1) {
		m_wobbling.z = 1.1f + m_wobbling.y;
	}
	else {
		m_wobbling.z = y;
	}
//	placeWeapon(m_currentWeapon, glm::vec2(m_wobbling.x, m_wobbling.y - m_wobbling.z));
	placeWeapon(m_currentWeapon, glm::vec2(0, dfy));
}
*/

/**
 * when the player gets hit by a laser
 */
void DarkForces::Player::onHitBullet(gaMessage* message)
{
	// if the player gets hit by laser from storm/commando/officer
	gaEntity* shooter = static_cast<gaEntity*>(message->m_extra);
	const std::string& name = shooter->name();

	if (name.find("OFFCFIN") != std::string::npos ||
		name.find("COMMANDO") != std::string::npos ||
		name.find("STORMFIN") != std::string::npos) 
	{
		//sendMessage(gaMessage::Action::PLAY_SOUND, DarkForces::Sounds::PLAYER_HIT_BY_STORM_COMMANDO_OFFICER);
	}
}

/**
 * a bullet passed by the player
 */
void DarkForces::Player::onBulletMiss(gaMessage* mmessage)
{
	sendMessage(gaMessage::Action::PLAY_SOUND, DarkForces::Sounds::PLAYER_NEARLY_HIT);
}

/**
 * Togle the gogles
 */
void DarkForces::Player::onTogleGogle(gaMessage* mmessage)
{
	DarkForces::Headlight* item = dynamic_cast<DarkForces::Headlight*>(m_inventory.get("goggles"));
	if (item) {
		if (item->on()) {
			item->set(false);
			g_dfHUD->setGoggle(false);
		}
		else {
			item->set(true);
			g_dfHUD->setGoggle(true);
		}
	}
}

/**
 * Togle the headlight
 */
void DarkForces::Player::onTogleHeadlight(gaMessage* mmessage)
{
	DarkForces::Headlight* item = dynamic_cast<DarkForces::Headlight*>(m_inventory.get("headlight"));
	if (item) {
		if (item->on()) {
			item->set(false);
			g_dfHUD->setHeadlight(false);
		}
		else {
			item->set(true);
			g_dfHUD->setHeadlight(true);
		}
	}
}

/**
 * display number of ammo
 */
void DarkForces::Player::onAmmo(gaMessage* message)
{
	// only the entity player catches the AMMO message to passthrough to the hud
	sendMessage("hud", DarkForces::Message::AMMO, message->m_value);
}

/**
 * display number of shield
 */
void DarkForces::Player::onShield(gaMessage* message)
{
	// only the entity player catches the AMMO message to passthrough to the hud
	sendMessage("hud", DarkForces::Message::SHIELD, message->m_value);
}

/**
 * display live status
 */
void DarkForces::Player::onLife(gaMessage* message)
{
	// only the entity player catches the LIFE message to passthrough to the hud
	sendMessage("hud", DarkForces::Message::LIFE, message->m_value);
}

/**
 * add item on the PDA
 */
void DarkForces::Player::onAddItem(gaMessage* message)
{
	sendMessage("pda", gaMessage::ADD_ITEM, message->m_value, message->m_extra);
}

/**
 * display the PDA
 */
void DarkForces::Player::onShowPDA(gaMessage* message)
{
	// only the entity player catches the LIFE message to passthrough to the hud
	sendMessage("pda", DarkForces::Message::PDA);
}

/**
 * display/hide the automap
 */
void DarkForces::Player::onShowAutomap(gaMessage* message)
{
	sendMessage(DarkForces::Message::AUTOMAP);
}

/**
 * when a goal is complete
 */
void DarkForces::Player::onCompleteGoal(gaMessage* message)
{
	sendMessage("pda", DarkForces::Message::COMPLETE, message->m_value);
}

/**
 * when the screen gets resized
 */
void DarkForces::Player::onScreenResize(gaMessage* message)
{
	g_dfHUD->setScreenSize(message->m_fvalue);
}

/**
 * add component on screen
 */
void DarkForces::Player::setScene(fwScene* scene)
{
	scene->addMesh2D(m_weapon.getImage());
}

/**
 * let an entity deal with a situation
 */
void DarkForces::Player::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case gaMessage::KEY_UP:
		if (message->m_value == GLFW_KEY_LEFT_CONTROL) {
			// single shot
			sendMessage(DarkForces::Message::STOP_FIRE, 0, &m_direction);
		}
		break;

	case gaMessage::Action::KEY:
		switch (message->m_value) {
		case GLFW_KEY_LEFT_CONTROL:
			// auto fire
			sendMessage(DarkForces::Message::START_FIRE, 0, &m_direction);
			break;

		case GLFW_KEY_F1:
			onShowPDA(message);
			break;

		case GLFW_KEY_F3:
			onTogleGogle(message);
			break;

		case GLFW_KEY_F5:
			onTogleHeadlight(message);
			break;

		case GLFW_KEY_TAB:
			onShowAutomap(message);
			break;

		default:
			if (g_WeaponKeys.count(message->m_value) > 0) {
				onChangeWeapon(message->m_value);
			}
		}
		break;

	case DarkForces::Message::FIRE:
		onFire(message);
		break;

	case gaMessage::Action::MOVE:
		onMove(message);
		break;

	case gaMessage::Action::ALARM:
		onAlarm(message);
		break;

		/*
	case gaMessage::Action::LOOK_AT:
		onLookAt(message);
		break;
		*/

	case gaMessage::Action::BULLET_HIT:
		onHitBullet(message);
		break;

	case gaMessage::Action::BULLET_MISS:
		onBulletMiss(message);
		break;

	case DarkForces::Message::AMMO:
		onAmmo(message);
		break;

	case DarkForces::Message::SHIELD:
		onShield(message);
		break;

	case DarkForces::Message::LIFE:
		onLife(message);
		break;

	case gaMessage::ADD_ITEM:
		onAddItem(message);
		break;

	case DarkForces::Message::COMPLETE:
		onCompleteGoal(message);
		break;

	case gaMessage::Action::SCREEN_RESIZE:
		onScreenResize(message);
		break;
	}

	gaActor::dispatchMessage(message);
}

//***********************************

/**
 * return a record of the entity state (for debug)
 */
uint32_t DarkForces::Player::recordState(void* record)
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
void DarkForces::Player::loadState(void* record)
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

//***********************************

/**
 * Add dedicated component debug the entity
 */
void DarkForces::Player::debugGUIChildClass(void)
{
	gaActor::debugGUIChildClass();

	if (ImGui::TreeNode("dfActor")) {
		ImGui::TreePop();
	}
}