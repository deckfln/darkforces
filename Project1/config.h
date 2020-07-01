#pragma once

#include "gaEngine/gaDebug.h"

const std::string ROOT_FOLDER = "c:/dev/project1/project1/";
const std::string DATA_FOLDER = ROOT_FOLDER + "/data/";

#define BENCHMARK

#define DEBUG

enum {
	REDUCED_DEBUG,
	LOW_DEBUG,
	HIGH_DEBUG,
	FULL_DEBUG
};