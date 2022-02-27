#include "dfParseGoal.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <map>

#include "dfFileSystem.h"
#include "dfParseINF.h"
#include "dfObject.h"

static dfLogic g_dfItemGoal[] = {
	dfLogic::PLANS,
	dfLogic::PHRIK,
	dfLogic::NAVA,
	dfLogic::DATATAPE,
	dfLogic::DT_WEAPON,
	dfLogic::PILE
};

/**
 * parse the .GOL file
 */
void DarkForces::Goals::parse(const std::string& file)
{
	int size = 0;

	char* sec = g_dfFiles->load(DF_DARK_GOB, file + ".GOL", size);
	std::istringstream infile(sec);
	std::string line;
	std::map<std::string, std::string> tokenMap;

	uint32_t goalID;
	uint32_t id;

	while (std::getline(infile, line)) {
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "GOAL:") {
			goalID = std::stoi(tokens[1]);

			if (tokens[2] == "ITEM:") {
				id = std::stoi(tokens[3]);
				m_goals.push_back( { false, id, 0 } );
			}
			else if (tokens[2] == "TRIG:") {
				id = std::stoi(tokens[3]);
				m_goals.push_back({ false, id, 1 });
			}
			else {
				printf("parseGoal:unknown token %s in %s\n", tokens[2].c_str(), line.c_str());
			}
		}
	}
}

/**
 * tick goals
 */
void DarkForces::Goals::onTrigger(gaMessage* message)
{
	for (auto& goal : m_goals) {
		if (goal.m_id == message->m_value) {
			m_completed++;
		}
	}
}

/**
 *
 */
DarkForces::Goals::Goals(const std::string& file):
	gaEntity(DarkForces::ClassID::_Goals, "DarkForces:Goals")
{
	m_physical = false;
	m_gravity = false;

	parse(file);
}

/**
 * let an entity deal with a situation
 */
void DarkForces::Goals::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::TRIGGER:
		onTrigger(message);
	}
}
