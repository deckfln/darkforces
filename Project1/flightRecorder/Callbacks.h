#pragma once

extern void* (*g_Callbacks[])(void*);

#include "classes.h"

#include "Entity.h"
#include "Actor.h"
#include "frElevator.h"
#include "frBullet.h"

namespace flightRecorder {
	void init_callbacks(void) {
		g_Callbacks[(int)TYPE::ENTITY] = frCreate_Entity;
		g_Callbacks[(int)TYPE::ENTITY_ACTOR] = frCreate_Actor;
		g_Callbacks[(int)TYPE::DF_ENTITY_ELEVATOR] = frCreate_Elevator;
		g_Callbacks[(int)TYPE::DF_ENTITY_BULLET] = frCreate_Bullet;
	}
}