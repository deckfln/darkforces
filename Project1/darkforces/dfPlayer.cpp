#include "dfPlayer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../gaEngine/World.h"

#include "dfSounds.h"
#include "dfFileSystem.h"
#include "dfSector.h"
#include "dfLevel.h"
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
	addComponent(&m_health);
	addComponent(&m_ammo);
	addComponent(&m_text);

	m_defaultAI.setClass("player");
	sendMessage(DarkForces::Message::AMMO, m_clip.energy());
	sendMessage("pda", gaMessage::ADD_ITEM, 0, &m_pistol);
	sendMessage(DarkForces::Message::CHANGE_WEAPON, 0, &m_pistol);
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

	// confirm the weapon change to components
	sendMessage(DarkForces::Message::CHANGE_WEAPON, 0, weapon);
}

/**
 * when the player moves
 */
void DarkForces::Player::onMove(gaMessage* message)
{
	// extract our sector and get the lighthing for the hud
	dfSector* sector = static_cast<dfLevel*>(g_gaLevel)->findSector(position());
	if (sector != nullptr) {
		float ambient = sector->ambient() / 32.0f;
		sendMessage(DarkForces::Message::AMBIENT, 0, ambient);
		//g_dfHUD->setAmbient(ambient);
	}
}

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
			sendMessage("*", DarkForces::Message::GOGGLES_ON);
		}
		else {
			item->set(true);
			sendMessage("*", DarkForces::Message::GOGGLES_OFF);
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
			sendMessage("*", DarkForces::Message::HEADLIGHT_ON);
		}
		else {
			item->set(true);
			sendMessage("*", DarkForces::Message::HEADLIGHT_OFF);
		}
	}
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
 * add component on screen
 */
void DarkForces::Player::setScene(fwScene* scene)
{
	scene->addMesh2D(m_weapon.getImage());
	scene->addMesh2D(m_health.getImage());
	scene->addMesh2D(m_ammo.getImage());
	scene->addMesh2D(m_text.getImage());
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

	case gaMessage::Action::MOVE:
		onMove(message);
		break;

	case gaMessage::Action::BULLET_HIT:
		onHitBullet(message);
		break;

	case gaMessage::Action::BULLET_MISS:
		onBulletMiss(message);
		break;

	case gaMessage::ADD_ITEM:
		onAddItem(message);
		break;

	case DarkForces::Message::COMPLETE:
		onCompleteGoal(message);
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