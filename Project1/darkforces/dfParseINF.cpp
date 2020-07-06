#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "../config.h"
#include "dfParseINF.h"
#include "dfLogicStop.h"
#include "dfLogicElevator.h"
#include "dfFileSystem.h"
#include "dfVOC.h"

static dfFileSystem* dfFiles = nullptr;
static std::map<std::string, dfVOC*> g_cachedVOC;

std::vector<std::string>& dfParseTokens(std::string& line, std::map<std::string, std::string>& tokenMap)
{
	static std::vector<std::string> tokens;

	tokens.clear();
	tokenMap.clear();

	// ignore comment
	if (line[0] == '#') {
		return tokens;
	}

	unsigned char c;
	int start = -1;

	int size = line.length();
	if (line[size - 1] == '\r') {
		// ignore leading \r
		size--;
	}

	for (int i = 0; i < size; i++) {
		c = line[i];

		// ignore comment #
		if (c == '#') {
			break;
		}

		// ignore comment /* */
		if (c == '/' && line[i + 1] == '*') {
			for (i = i + 1; i < size - 1; i++) {
				if (line[i] == '*' && line[i + 1] == '/') {
					i += 2;
					break;
				}
			}
			// reached the end of the line
			if (i >= size) {
				break;
			}

			c = line[i];
		}

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
		tokens.push_back(line.substr(start, size - start));
	}

	if (tokens.size() > 0) {
		for (auto i = 0; i < tokens.size() - 1; i++) {
			if (tokens[i].find(':') != std::string::npos) {
				tokenMap[tokens[i]] = tokens[i + 1];
			}
		}
	}

	return tokens;
}

dfParseINF::dfParseINF(dfFileSystem* fs, std::string file)
{
	int size;

	dfFiles = fs;

	char* sec = fs->load(DF_DARK_GOB, file + ".INF", size);
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
}

/**
 * Parse the message tokens
 */
static gaMessage* parseMessage(std::vector<std::string>& tokens)
{
	int s = tokens.size();
	const std::string done = "done";
	const std::string gotostop = "goto_stop";

	int action = -1;
	int value;
	std::string client;

	switch (s) {
	case 5:
		// message: 1 elev3-5 goto_stop 0
		if (tokens[3] == gotostop) {
			action = DF_MESSAGE_GOTO_STOP;
			value = std::stoi(tokens[4]);
			client = tokens[2];
		}
		break;
	case 4:
		// message: 0 ext(6) done
		if (tokens[3] == done) {
			// split sector / wall on tokens[2];
			action = DF_MESSAGE_DONE;
			value = std::stoi(tokens[1]);
			client = tokens[2];
		}
		else if (tokens[3] == "m_trigger") {
			action = DF_MESSAGE_TRIGGER;
			value = std::stoi(tokens[1]);
			client = tokens[2];
		}
		break;
	case 3:
		// message: goto_stop 1
		if (tokens[1] == gotostop) {
			action = DF_MESSAGE_GOTO_STOP;
			value = std::stoi(tokens[2]);
		}
	}

	// detect failed parsing
	if (action == -1) {
		std::string m = "";
		for (auto s : tokens) {
			m += " " + s;
		}
		std::cerr << "gaMessage::parse " << m << " not implemented" << std::endl;
		return nullptr;
	}

	return new gaMessage(action, value, client);
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

void dfParseINF::parseSector(std::istringstream& infile, std::string& sector)
{
	std::string line, dump;
	bool start = false;

	dfLogicElevator* elevator = nullptr;
	dfLogicTrigger* trigger = nullptr;
	dfLogicStop* stop = nullptr;
	std::map<std::string, std::string> tokenMap;

	int nbStops = -1;

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

		if (tokens[0] == "seq") {
			// pass
		}
		else if (tokens[0] == "seqend") {
			if (elevator) {
				m_elevators.push_back(elevator);
			}
			else if (trigger) {
				trigger->config();
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
					trigger = new dfLogicTrigger(tokens[2], sector);
				}
			}
		}
		else if (tokens[0] == "speed:") {
			if (elevator) {
				elevator->speed(std::stof(tokens[1]));
			}
		}
		else if (tokens[0] == "center:") {
			if (elevator) {
				elevator->center(-std::stof(tokens[1]), std::stof(tokens[2]));	//inverse X
			}
		}
		else if (tokens[0] == "angle:") {
			if (elevator) {
				elevator->angle(std::stof(tokens[1]));
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
		else if (tokens[0] == "key:") {
			if (elevator) {
				elevator->keys(tokens[1]);
			}
			else if (trigger) {
				std::cerr << "dfParseINF::parseSector kkey not implmented for  triggers" << std::endl;
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
					stop->message( parseMessage(tokens) );
				}
				else {
					std::cerr << "dfParseINF::parseSector stop/messages not in order for " << sector << " stop #" << nbStops << " message #" << i << std::endl;
				}
			}
			else if (trigger) {
				trigger->message( parseMessage(tokens) );
			}
		}
		else if (tokens[0] == "stop:") {
			stop = new dfLogicStop(elevator);
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
		else if (tokens[0] == "sound:") {
			if (elevator) {
				int effect = std::stoi(tokens[1]);
				if (tokens[2] != "0") {
					if (g_cachedVOC.count(tokens[2]) == 0) {
						g_cachedVOC[tokens[2]] = new dfVOC(dfFiles, tokens[2]);
					}

					elevator->sound(effect - 1, g_cachedVOC[tokens[2]]);
				}
				else {
					elevator->sound(effect - 1, nullptr);	// silent the default sound
				}
			}
		}
	}
}

void dfParseINF::parseLine(std::istringstream& infile, std::string &sector, int wallIndex)
{
	std::string line, dump;
	bool start = false;

	std::string kind;
	dfLogicTrigger* trigger = nullptr;
	std::map<std::string, std::string> tokenMap;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);

		if (tokens[0] == "seq") {
			// pass
		}
		else if (tokens[0] == "seqend") {
			if (trigger) {
				trigger->config();
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
		else if (tokens[0] == "sound:") {
			std::cerr << "dfParseINF::parseLine sound: not implemented" << std::endl;
		}
	}
}
