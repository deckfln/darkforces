#pragma once

#include <string>
#include <list>

#include "dfElevator.h"
#include "dfLogicTrigger.h"

class dfFileSystem;
class dfLevel;

std::vector<std::string>& dfParseTokens(std::string& line, std::map<std::string, std::string>& tokenMap);

class dfParseINF
{
	int m_items = 0;
	dfLevel* m_level = nullptr;
	void parseSector(std::istringstream& infile, const std::string& name);
	void parseLine(std::istringstream& infile, const std::string& sector, int wallIndex);

public:
	std::list <dfElevator*> m_elevators;
	std::list <dfLogicTrigger* > m_triggers;

	dfParseINF(dfFileSystem* fs, const std::string& file, dfLevel *level);
	~dfParseINF(void);
};