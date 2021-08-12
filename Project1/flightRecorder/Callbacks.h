#pragma once

extern void* (*g_Callbacks[])(void*);

#include "classes.h"

#include "Entity.h"
#include "Actor.h"
#include "frBullet.h"
#include "../darkforces/flightRecorder/frElevator.h"
#include "../darkforces/flightRecorder/frObject.h"
#include "../darkforces/flightRecorder/frSpriteAnimated.h"

namespace flightRecorder {
	void init_callbacks(void) {
		g_Callbacks[(int)TYPE::ENTITY] = frCreate_Entity;
		g_Callbacks[(int)TYPE::ENTITY_ACTOR] = frCreate_Actor;
		g_Callbacks[(int)TYPE::DF_ENTITY_ELEVATOR] = frCreate_Elevator;
		g_Callbacks[(int)TYPE::DF_ENTITY_BULLET] = frCreate_Bullet;
		g_Callbacks[(int)TYPE::DF_ENTITY_OBJECT] = frCreate_dfObject;
		g_Callbacks[(int)TYPE::DF_ENTITY_SPRITEANIMATED] = frCreate_dfSpriteAnimated;
	}
}