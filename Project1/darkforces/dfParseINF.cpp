#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "../config.h"
#include "dfParseINF.h"
#include "dfLogicStop.h"
#include "dfLogicElevator.h"

std::vector<std::string>& dfParseTokens(std::string& line)
{
	static std::vector<std::string> tokens;

	tokens.clear();

	unsigned char c;
	int start = -1;

	for (unsigned int i = 0; i < line.length(); i++) {
		c = line[i];
		if (c == ' ' || c == '\t') {
			if (start >= 0) {
				tokens.push_back(line.substr(start, i - start));
				start = -1;
			}
		}
		else {
			if (start == -1) {
				start = i;
			}
		}
	}
	if (start >= 0) {
		tokens.push_back(line.substr(start));
	}

	return tokens;
}

dfParseINF::dfParseINF(std::string file)
{
	std::ifstream infile(ROOT_FOLDER + "/" + file + ".INF");
	std::string line, dump;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);

		if (tokens[0] == "items") {
			m_items = std::stoi(tokens[1]);
		}
		else if (tokens[0] == "item:") {
			if (tokens[1] == "sector") {
				parseSector(infile, tokens[3]);
			}
			else if (tokens[1] == "line") {
				parseLine(infile, tokens[3], std::stoi(tokens[5]));
			}
		}
	}
	infile.close();
}

dfParseINF::~dfParseINF(void)
{
	for (auto elevator : m_elevators) {
		delete elevator;
	}
	for (auto trigger: m_triggers) {
		delete trigger;
	}
}

void dfParseINF::parseSector(std::ifstream& infile, std::string& sector)
{
	std::string line, dump;
	bool start = false;

	dfLogicElevator* elevator = nullptr;
	dfLogicTrigger* trigger = nullptr;
	dfLogicStop* stop = nullptr;

	int nbStops = -1;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);

		if (tokens[0] == "seq") {
			// pass
		}
		else if (tokens[0] == "seqend") {
			if (elevator) {
				m_elevators.push_back(elevator);
			}
			else if (trigger) {
				m_triggers.push_back(trigger);
			}
			break;
		}
		else if (tokens[0] == "class:") {
			if (tokens[1] == "elevator") {
				elevator = new dfLogicElevator(tokens[2], sector);
			}
			else if (tokens[1] == "trigger") {
				if (tokens.size() == 2) {
					std::cerr << "*class: trigger* not implemented" << std::endl;
				}
				else {
					trigger = new dfLogicTrigger(tokens[2], sector, -1);
				}
			}
		}
		else if (tokens[0] == "speed:") {
			if (elevator) {
				elevator->speed(std::stoi(tokens[1]));
			}
		}
		else if (tokens[0] == "center:") {
			if (elevator) {
				elevator->center(-std::stoi(tokens[1]), std::stoi(tokens[2]));	//inverse X
			}
		}
		else if (tokens[0] == "event_mask:") {
			if (elevator) {
				elevator->eventMask(std::stoi(tokens[1]));
			}
			else if (trigger) {
				trigger->eventMask(std::stoi(tokens[1]));
			}
		}
		else if (tokens[0] == "client:") {
			if (trigger) {
				trigger->client(tokens[1]);
			}
		}
		else if (tokens[0] == "message:") {
			if (stop) {
				int i = std::stoi(tokens[1]);
				if (i == nbStops) {
					stop->message(tokens);
				}
				else {
					std::cerr << "dfParseINF::parseSector stop/messages not in order for " << sector << " stop #" << nbStops << " message #" << i << std::endl;
				}
			}
			else if (trigger) {
				trigger->message(tokens);
			}
		}
		else if (tokens[0] == "stop:") {
			stop = new dfLogicStop();
			nbStops++;
			char* p;

			// value 1
			// | [time] time in sec that elevator remains at stop 
			// | hold elevator will remain at stop indefinitely 
			// | terminate elevator will stay at the stop permanently 
			// | complete mission will be complete when elev arrives at stop
			const char* string = tokens[1].c_str();
			float converted = strtof(string, &p);

			if (tokens[1][0] == '@') {
				stop->relative(std::stof(tokens[1].substr(1)));
			}
			else if (p == string) {
				// conversion failed because the input wasn't a number
				stop->sector(tokens[1]);
			}
			else {
				// use converted
				stop->absolute(converted);
			}

			// value 2
			// | [time] time in sec that elevator remains at stop 
			// | hold elevator will remain at stop indefinitely 
			// | terminate elevator will stay at the stop permanently 
			// | complete mission will be complete when elev arrives at stop
			string = tokens[2].c_str();
			converted = strtof(tokens[2].c_str(), &p);

			if (p == string) {
				// conversion failed because the input wasn't a number
				stop->action(tokens[2]);
			}
			else {
				// use converted
				stop->time(converted);
			}

			if (elevator) {
				elevator->addStop(stop);
			}
		}
	}
}

void dfParseINF::parseLine(std::ifstream& infile, std::string &sector, int wallIndex)
{
	std::string line, dump;
	bool start = false;
	dfLogicStop stop;

	std::string kind;
	dfLogicTrigger* trigger = nullptr;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);

		if (tokens[0] == "seq") {
			// pass
		}
		else if (tokens[0] == "seqend") {
			if (trigger) {
				m_triggers.push_back(trigger);
			}
			break;
		}
		else if (tokens[0] == "class:") {
			if (tokens[1] == "trigger") {
				trigger = new dfLogicTrigger(tokens[2], sector, wallIndex);
			}
		}
		else if (tokens[0] == "client:") {
			if (trigger) {
				trigger->client(tokens[1]);
			}
		}
		else if (tokens[0] == "event_mask:") {
			if (trigger) {
				trigger->eventMask(std::stoi(tokens[1]));
			}
		}
		else if (tokens[0] == "message:") {
			std::cerr << "dfParseINF::parseLine message: not implemented" << std::endl;
		}
	}
}
