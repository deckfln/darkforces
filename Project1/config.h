#pragma once

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
 * Categoty of entities
 */
enum {
	DF_ENTITY_OBJECT,	// static objects (armor, energy, rock)
	DF_ENTITY_TRIGGER,	// all sort of triggers
	DF_ENTITY_ELEVATOR,	// moving doors and platforms
	DF_ENTITY_BULLET
};

/**
 * type of component
 */
enum {
	DF_COMPONENT_ACTOR = 1024,
	DF_COMPONENT_LOGIC
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
};