#include "dfWeapon.h"

#include "../gaEngine/World.h"
#include "prefab/dfEnergy.h"
#include "dfObject/dfSprite/dfRifle.h"

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
	200,		// default energy on new pistol
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
	200,	// default energy on new riffle
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

	switch (m_kind) {
	case DarkForces::Weapon::Kind::Pistol:
		m_object = new DarkForces::Prefab::Energy(p, (uint32_t)message->m_fvalue);
		break;
	case DarkForces::Weapon::Kind::Rifle:
		m_object = new DarkForces::Sprite::Rifle(p, 1.0f, (uint32_t)message->m_fvalue);
		m_object->logic(dfLogic::ITEM_RIFLE);
		break;
	}
	m_object->item(this);
	m_object->physical(false);	// objects can be traversed and are not subject to gravity
	m_object->gravity(false);

	g_gaWorld.addClient(m_object);
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