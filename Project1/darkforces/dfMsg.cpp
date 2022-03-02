#include "dfMsg.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include "dfFileSystem.h"
#include "dfParseINF.h"

#include "dfObject.h"

DarkForces::Msgs g_dfMsg;

void DarkForces::Msgs::Parse(const std::string& file)
{
	int32_t size = 0;
	char* sec = g_dfFiles->load(DF_DARK_GOB, file, size);
	std::istringstream infile(sec);
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens.size() == 3) {
			uint32_t id = std::stoi(tokens[0]);
			uint32_t importance = std::stoi(tokens[1]);
			m_msgs[id] = Msg(id, importance, tokens[2]);
		}
	}
}