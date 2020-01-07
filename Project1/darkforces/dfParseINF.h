#pragma once

#include <string>
#include <list>

#include "dfLogicElevator.h"
#include "dfLogicTrigger.h"

std::vector<std::string>& dfParseTokens(std::string& line);

class dfParseINF
{
	int m_items = 0;

	void parseSector(std::ifstream& infile, std::string& name);
	void parseLine(std::ifstream& infile, std::string& sector, int wallIndex);

public:
	std::list <dfLogicElevator*> m_elevators;
	std::list <dfLogicTrigger* > m_triggers;

	dfParseINF(std::string file);
	~dfParseINF(void);
};