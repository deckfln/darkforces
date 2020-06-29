#pragma once

#include <string>

void gaDebugLog(int level, const std::string& function, const std::string message);
extern int g_debugLevel;