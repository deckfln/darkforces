#include "gaDebug.h"

#include "../config.h"

#include <stdio.h>
#include <iostream>
#include <ctime>  

int g_debugLevel = HIGH_DEBUG;

void gaDebugLog(int level, const std::string& function, const std::string message)
{
	if (level < g_debugLevel) {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        localtime_s(&tstruct, &now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

		std::cerr << buf << " " << " " << function << ":" << message << std::endl;
	}
}
