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
		uint32_t size;						// size of the record
		uint32_t classID;					// class gaEntity
		uint32_t id;						// Entity ID
		char className[32];					// class name
		char name[64];						// name of the entity
		Object3D object3D;					// source object
		GameEngine::Transform transforms;	// transforms to move the object
		AABBox modelAABB;					// model space AABB
		AABBox worldBounding;				// AABB bounding box in world opengl space
		time_t animation_time;				// elapsed time when running animation
		bool timer;
		bool processMessages;
		uint32_t nbComponents;				// number of components at the end of the record
	};
}
