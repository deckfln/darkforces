#pragma once
#include <string>
#include <vector>

#include "dfSector.h"

class dfLevel
{
	std::string m_name;
	std::string m_level;
	std::vector<dfSector *> m_sectors;

public:
	dfLevel(std::string name);
	~dfLevel();
};