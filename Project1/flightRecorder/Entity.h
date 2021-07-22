#pragma once

#include <time.h>

#include "classes.h"
#include "Object3D.h"
#include "AABBox.h"
#include "../gaEngine/gaMessage.h"

namespace flightRecorder {
	/*
	 * basic structure to export the object
	 */
	struct Entity {
		int size = 0;						// size of the record
		TYPE classID;						// class gaEntity
		char name[64];						// name of the entity
		Object3D object3D;					// source object
		GameEngine::Transform transforms;	// transforms to move the object
		AABBox modelAABB;					// model space AABB
		AABBox worldBounding;				// AABB bounding box in world opengl space
		time_t animation_time;				// elapsed time when running animation
	};
}