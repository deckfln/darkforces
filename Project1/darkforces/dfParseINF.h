#pragma once

#include <string>
#include <list>

#include "dfElevator.h"
#include "dfLogicTrigger.h"

class dfFileSystem;

std::vector<std::string>& dfParseTokens(std::string& line, std::map<std::string, std::string>& tokenMap);

class dfParseINF
{
	int m_items = 0;

	void parseSector(std::istringstream& infile, std::string& name);
	void parseLine(std::istringstream& infile, std::string& sector, int wallIndex);

public:
	std::list <dfElevator*> m_elevators;
	std::list <dfLogicTrigger* > m_triggers;

	dfParseINF(dfFileSystem* fs, std::string file);
	~dfParseINF(void);
};