#include "dfWeapon.h"

DarkForces::Weapon g_concussion = DarkForces::Weapon(
	DarkForces::Weapon::Kind::Concussion,
	"Concussion",
	"CONCUSS5.VOC",
	100,
	0.1f,
	5,
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
	1000,
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
	{ "rifle1.bm", "rifle2.bm" },
	{ nullptr, nullptr },
	glm::vec2(0.17, -0.83)
);

