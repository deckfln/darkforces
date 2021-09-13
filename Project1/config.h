#pragma once

#include <map>
#include "gaEngine/gaDebug.h"

const std::string ROOT_FOLDER = "/dev/project1/project1/";
const std::string DATA_FOLDER = ROOT_FOLDER + "/data/";

// #define BENCHMARK

enum {
	REDUCED_DEBUG,
	LOW_DEBUG,
	HIGH_DEBUG,
	FULL_DEBUG
};

/**
 * type of component
 */
enum {
	DF_COMPONENT_ACTOR = 1024,
	DF_COMPONENT_LOGIC,
	DF_COMPONENT_AI,
	DF_COMPONENT_INF,
	DF_COMPONENT_INF_ELEVATOR,
	DF_COMPONENT_INF_ELEVATOR_LIGHT,
	DF_COMPONENT_INF_ELEVATOR_MOVE_FLOOR,
	DF_COMPONENT_INF_ELEVATOR_MOVE_CEILING,
	DF_COMPONENT_INF_ELEVATOR_INV,
	DF_COMPONENT_INF_ELEVATOR_DOOR,
	DF_COMPONENT_INF_ELEVATOR_BASIC,
	DF_COMPONENT_INF_ELEVATOR_MORPH_SPIN1,
	DF_COMPONENT_INF_ELEVATOR_MORPH_SPIN2,
	DF_COMPONENT_INF_ELEVATOR_MORPH_MOVE1,
	DF_COMPONENT_TRIGGER
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
		HIT_BULLET,			// the object was hit by a bullet
		END_LOOP,			// the animation loop stopped
		DIES,				// the object died
		STATE,				// change the state of the object
		PICK_RIFLE_AND_BULLETS,
		ADD_BATTERY,
		EVENT,				// send events to sectors
		FIRE,				// fire a gun
		SatNav_Wait			// ticks for the DF satnav
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

	enum class Keys {
		NONE = 0,
		RED = 1,
		BLUE = 2,
		YELLOW = 4
	};
}

const int DF_SHIELD_ENERGY = 100;
const int DF_ENERGY_ENERGY = 15;
const int DF_BATTERY_ENERGY = 100;
