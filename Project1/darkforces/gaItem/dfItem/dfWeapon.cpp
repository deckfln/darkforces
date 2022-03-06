#include "dfWeapon.h"

#include <imgui.h>

#include "../../../gaEngine/World.h"
#include "../../../gaEngine/gaComponent/gaCInventory.h"

#include "../../prefab/dfEnergy.h"
#include "../../dfObject/dfSprite/dfRifle.h"
#include "../../gaItem/dfItem/dfEnergyClip.h"

/**
 *
 */
DarkForces::Weapon g_concussion = DarkForces::Weapon(
	DarkForces::Weapon::Kind::Concussion,
	"Concussion",
	"CONCUSS5.VOC",
	100,
	0.1f,
	5,
	1,
	100,
	{ "concuss1.bm" },
	{ nullptr },
	glm::vec2(0.0, 0.0)
);
DarkForces::Weapon g_FusionCutter = DarkForces::Weapon(
	DarkForces::Weapon::Kind::FusionCutter,
	"FusionCutter",
	"FUSION1.VOC",
	100,
	0.1f,
	5,
	1,
	100, 
	{ "fusion1.bm" },
	{ nullptr },
	glm::vec2(0.0, 0.0)
);
DarkForces::Weapon g_Missile = DarkForces::Weapon(
	DarkForces::Weapon::Kind::Missile,
	"Missile",
	"MISSILE1.VOC",
	100,
	0.1f,
	5,
	1,
	100,
	{ "assault1.bm" },
	{ nullptr },
	glm::vec2(0.0, 0.0)
);
DarkForces::Weapon g_MortarGun = DarkForces::Weapon(
	DarkForces::Weapon::Kind::MortarGun,
	"MortarGun",
	"MORTAR2.VOC",
	100,
	0.1f,
	5,
	1,
	100,
	{ "mortar1.bm" },
	{ nullptr },
	glm::vec2(0.0, 0.0)
);
DarkForces::Weapon g_Pistol = DarkForces::Weapon(
	DarkForces::Weapon::Kind::Pistol,
	"Pistol",
	"PISTOL-1.VOC",
	10,
	0.05f,
	500,
	100,		// default energy on new pistol
	200,		// maximum energy on new pistol
	{ "pistol1.bm", "pistol2.bm", "pistol3.bm" },
	{ nullptr, nullptr, nullptr },
	glm::vec2(0.3, -0.9)
);
DarkForces::Weapon g_PlasmaCannon = DarkForces::Weapon(
	DarkForces::Weapon::Kind::PlasmaCannon,
	"PlasmaCannon",
	"PLASMA4.VOC",
	10,
	0.1f,
	5,
	1,
	100,
	{ "assault1.bm" },
	{ nullptr },
	glm::vec2(0.0, 0.0)
);
DarkForces::Weapon g_Repeater = DarkForces::Weapon(
	DarkForces::Weapon::Kind::Repeater,
	"Repeater",
	"REPEATER.VOC",
	10,
	0.1f,
	5,
	1,
	100,
	{ "autogun1" },
	{ nullptr },
	glm::vec2(0.2, 0.8)
);
DarkForces::Weapon g_Rifle = DarkForces::Weapon(
	DarkForces::Weapon::Kind::Rifle,
	"Rifle",
	"RIFLE-1.VOC",
	15,
	0.2f,
	200,
	100,		// default energy on new riffle
	200,		// maximum energy 
	{ "rifle1.bm", "rifle2.bm" },
	{ nullptr, nullptr },
	glm::vec2(0.17, -0.83)
);

/**
 *
 */
void DarkForces::Weapon::onDropItem(gaMessage* message)
{
	// constructor of a sprite expects a level space
	glm::vec3 p;
	glm::vec3 p1 = message->m_v3value + glm::vec3(2.0 * message->m_fvalue / (rand() % 10) - message->m_fvalue, 0, 2.0 * message->m_fvalue / (rand() % 10) - message->m_fvalue);
	dfLevel::gl2level(p1, p);

	// force drop of the clip
	DarkForces::EnergyClip* energy = dynamic_cast<DarkForces::EnergyClip*>(m_inventory->get("EnergyClip"));

	switch (m_kind) {
	case DarkForces::Weapon::Kind::Pistol:
		energy->drop(p);
		break;

	case DarkForces::Weapon::Kind::Rifle:
		// drop a rifle loaded with the content of the energyclip
		m_object = new DarkForces::Sprite::Rifle(p, 1.0f, (uint32_t)energy->energy());
		m_object->logic(dfLogic::ITEM_RIFLE);
		break;
	}

	if (m_object) {
		m_object->item(this);
		m_object->physical(false);	// objects can be traversed and are not subject to gravity
		m_object->gravity(false);

		GameEngine::World::add(m_object);
	}
}

/**
 *
 */
void DarkForces::Weapon::clone(DarkForces::Weapon* source)
{
	DarkForces::Item::Set(source->m_debug, dfLogic::WEAPON);
	m_kind = source->m_kind;
	m_debug = source->m_debug;
	m_fireSound = source->m_fireSound;
	m_damage = source->m_damage;
	m_recoil = source->m_recoil;
	m_rate = source->m_rate;
	m_HUDfiles = source->m_HUDfiles;
	m_HUDbmps = source->m_HUDbmps;
	m_HUDposition = source->m_HUDposition;
	m_energy = source->m_energy;
	m_maxEnergy = source->m_maxEnergy;
}

/**
 * // load the clip with energy
 */
void DarkForces::Weapon::loadClip(void)
{
	// weapon do store bullets in the inventory clip
	DarkForces::EnergyClip* clip = dynamic_cast<DarkForces::EnergyClip*>(m_inventory->get("EnergyClip"));
	clip->addEnergy(m_energy);
	clip->maxEnergy(m_maxEnergy);

}

/**
 *
 */
void DarkForces::Weapon::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::DROP_ITEM:
		onDropItem(message);
		break;
	}
}

/**
 * fire a bullet from the weapon
 */
uint32_t DarkForces::Weapon::decreaseEnergy(void) 
{
	// weapon do store bullets in the inventory clip
	DarkForces::EnergyClip* clip = dynamic_cast<DarkForces::EnergyClip*>(m_inventory->get("EnergyClip"));
	return clip->decreaseEnergy();
}

uint32_t DarkForces::Weapon::addEnergy(uint32_t v)
{
	// weapon do store bullets in the inventory clip
	DarkForces::EnergyClip* clip = dynamic_cast<DarkForces::EnergyClip*>(m_inventory->get("EnergyClip"));
	return clip->addEnergy(v);
}

uint32_t DarkForces::Weapon::energy(void)
{
	// weapon do store bullets in the inventory clip
	DarkForces::EnergyClip* clip = dynamic_cast<DarkForces::EnergyClip*>(m_inventory->get("EnergyClip"));
	return clip->energy();
}
