#pragma once

#include <map>
#include "gaEngine/gaDebug.h"

const std::string ROOT_FOLDER = "/dev/project1/project1/";
const std::string DATA_FOLDER = ROOT_FOLDER + "/data/";

#define BENCHMARK

#define DEBUG

enum {
	REDUCED_DEBUG,
	LOW_DEBUG,
	HIGH_DEBUG,
	FULL_DEBUG
};

/**
 * Category of entities
 */
enum {
	DF_ENTITY_OBJECT,	// static objects (armor, energy, rock)
	DF_ENTITY_TRIGGER,	// all sort of triggers
	DF_ENTITY_ELEVATOR,	// moving doors and platforms
	DF_ENTITY_BULLET,
	DF_ENTITY_SECTOR
};

/**
 * type of component
 */
enum {
	DF_COMPONENT_ACTOR = 1024,
	DF_COMPONENT_LOGIC,
	DF_COMPONENT_AI,
	DF_COMPONENT_INF
};

/**
 * List of messages
 */
enum {
	DF_MESSAGE_TRIGGER=1024,
	DF_MESSAGE_GOTO_STOP,
	DF_MESSAGE_DONE,
	DF_MESSAGE_ADD_SHIELD,
	DF_MESSAGE_ADD_ENERGY,
	DF_MESSAGE_HIT_BULLET,		// the object was hit by a bullet
	DF_MESSAGE_END_LOOP,		// the animation loop stopped
	DF_MESSAGE_DIES,			// the object died
	DF_MSG_STATE,				// change the state of the object
	DF_MSG_PICK_RIFLE_AND_BULLETS,
	DF_MSG_ADD_BATTERY,
	DF_MSG_EVENT				// send events to sectors
};

namespace DarkForces {
	enum Message {
		TRIGGER=1024,
		GOTO_STOP,
		DONE,
		ADD_SHIELD,
		ADD_ENERGY,
		HIT_BULLET,		// the object was hit by a bullet
		END_LOOP,		// the animation loop stopped
		DIES,			// the object died
		STATE,				// change the state of the object
		PICK_RIFLE_AND_BULLETS,
		ADD_BATTERY,
		EVENT				// send events to sectors
	};

	enum MessageEvent {
		CROSSLINE_FRONT = 1,	// Cross line from front side 
		CROSSLINE_BACK = 2,		// Cross line from back side 
		ENTER_SECTOR = 4,		// Enter sector
		LEAVE_SECTOR = 8,		// Leave sector 
		NUDGE_FRONT_INSIDE = 16,// Nudge line from front side / Nudge sector from inside 
		NUDGE_BACK_OUTSIE = 32,	// Nudge line from back side / Nudge sector from outside 
		EXPLOSION = 64,			// Explosion 
		SHOOT = 128,			// Shoot or punch line(see entity_mask) 
		LAND = 512				// Land on floor of sector
	};

	enum Keys {
		NONE = 0,
		RED = 1
	};
}

const int DF_SHIELD_ENERGY = 100;
const int DF_ENERGY_ENERGY = 15;
const int DF_BATTERY_ENERGY = 100;
