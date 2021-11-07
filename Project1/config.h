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

const int DF_SHIELD_ENERGY = 100;
const int DF_ENERGY_ENERGY = 15;
const int DF_BATTERY_ENERGY = 100;
