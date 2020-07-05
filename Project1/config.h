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
	DF_ENTITY_ACTOR,	// intelligent objects (player, enemy)
	DF_ENTITY_OBJECT,	// static objects (armor, energy, rock)
	DF_ENTITY_TRIGGER,	// all sort of triggers
	DF_ENTITY_ELEVATOR	// moving doors and platforms
};

