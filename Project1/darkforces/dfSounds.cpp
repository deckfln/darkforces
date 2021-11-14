#include "dfSounds.h"

static std::map<uint32_t, const std::string> g_dfSounds = {
	{ DarkForces::Sounds::WALL_HIT_LASER, "ex-tiny1.voc"},
	{ DarkForces::Sounds::PLAYER_HIT_BY_STORM_COMMANDO_OFFICER, "BOLTREF1.VOC"},
	{ DarkForces::Sounds::PLAYER_NEARLY_HIT, "LASRBY.VOC"},
	{ DarkForces::Sounds::MOUSEBOT_EEK, "EEEK-1.VOC"},
	{ DarkForces::Sounds::MOUSEBOT_DIE, "EEEK-3.VOC"},
	{ DarkForces::Sounds::STORM_COMMANDO_OFFICER_HIT_LASER, "ST-HRT-1.VOC"},
	{ DarkForces::Sounds::STORM_COMMANDO_OFFICER_DIE, "ST-DIE-1.voc"}
};

dfVOC* DarkForces::loadSound(uint32_t soundID)
{
	return loadVOC(g_dfSounds[soundID]);
}
