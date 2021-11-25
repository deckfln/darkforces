#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "../gaEngine/gaComponent/gaSound.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"

#include "../config.h"
#include "dfParseINF.h"
#include "dfLogicStop.h"
#include "dfFileSystem.h"
#include "dfVOC.h"
#include "dfMesh.h"
#include "dfSounds.h"
#include "dfComponent/InfStandardTrigger.h"
#include "dfComponent/Trigger.h"
#include "dfComponent/InfElevator/InfElevatorLight.h"
#include "dfComponent/InfElevator/InfElevatorVertical/InfElevatorInv.h"
#include "dfComponent/InfElevator/InfElevatorVertical/InfElevatorMoveFloor.h"
#include "dfComponent/InfElevator/InfElevatorVertical/InfElevatorBasic.h"
#include "dfComponent/InfElevator/InfElevatorVertical/InfElevatorMoveCeiling.h"
#include "dfComponent/InfElevator/InfElevatorRotate/InfElevatorMorphSpin1.h"
#include "dfComponent/InfElevator/InfElevatorRotate/InfElevatorMorphSpin2.h"
#include "dfComponent/InfElevator/InfElevatorRotate/InfElevatorMorphSpin2.h"
#include "dfComponent/InfElevator/InfElevatorHorizontal/InfElevatorMorphMove1.h"

#include "dfSector.h"

static dfFileSystem* dfFiles = nullptr;
static std::map<std::string, dfVOC*> g_cachedVOC;

/**
 * Create a default sound component
 */
static GameEngine::Component::Sound* newElevatorSound(DarkForces::Component::InfElevator *elevator, dfSector* pSector)
{
	// default elevators sounds
	static std::vector<std::vector<std::string>> g_Default_sounds = {
		{ { "elev2-1.voc", "elev2-2.voc", "elev2-3.voc"} },
		{ { "door2-1.voc", "door2-2.voc", "door2-3.voc"} },
	};

	static std::map<DarkForces::Component::InfElevator::Type, int> g_sound_evelators = {
	{DarkForces::Component::InfElevator::Type::INV, 1},
	{DarkForces::Component::InfElevator::Type::BASIC, 0},
	{DarkForces::Component::InfElevator::Type::MOVE_FLOOR, 0},
	{DarkForces::Component::InfElevator::Type::MOVE_CEILING, 1},
	{DarkForces::Component::InfElevator::Type::MORPH_SPIN1, 1},
	{DarkForces::Component::InfElevator::Type::MORPH_MOVE1, 1},
	{DarkForces::Component::InfElevator::Type::MORPH_SPIN2, 1},
	{DarkForces::Component::InfElevator::Type::DOOR, 2}
	};

	if (g_sound_evelators.count(elevator->type())) {
		// if there is a default sound
		GameEngine::Component::Sound* sound = dynamic_cast<GameEngine::Component::Sound *>(pSector->findComponent(gaComponent::SOUND));
		DarkForces::Component::InfElevator::Type type = elevator->type();
		int sounds = g_sound_evelators[type];

		for (auto i = 0; i < 3; i++) {
			const std::string& file = g_Default_sounds[sounds][i];
			sound->addSound(DarkForces::Sounds::ELEVATOR_START + i, loadVOC(file)->sound());
		}

		return sound;
	}

	return nullptr;
}

/**
 * replace an elevator sound
 */
static void addSound(dfSector* pSector, uint32_t code, const char* file)
{
	// if there is a default sound
	GameEngine::Component::Sound* sound = dynamic_cast<GameEngine::Component::Sound*>(pSector->findComponent(gaComponent::SOUND));

	sound->addSound(DarkForces::Sounds::ELEVATOR_START + code, loadVOC(file)->sound());
}

/**
 * remove an elevator sound
 */
static void removeSound(dfSector* pSector, uint32_t code)
{
	// if there is a default sound
	GameEngine::Component::Sound* sound = dynamic_cast<GameEngine::Component::Sound*>(pSector->findComponent(gaComponent::SOUND));

	sound->addSound(DarkForces::Sounds::ELEVATOR_START + code, nullptr);
}

/**
 *
 */
dfParseINF::dfParseINF(dfFileSystem* fs, const std::string& file, dfLevel *level)
{
	int size;

	dfFiles = fs;
	m_level = level;
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
		for (uint32_t i = 0; i < tokens.size() - 1; i++) {
			if (tokens[i].find(':') != std::string::npos) {
				tokenMap[tokens[i]] = tokens[i + 1];
			}
		}
	}

	return tokens;
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
			action = DarkForces::Message::GOTO_STOP;
			value = std::stoi(tokens[4]);
			client = tokens[2];
		}
		break;
	case 4:
		// message: 0 ext(6) done
		if (tokens[3] == done) {
			// split sector / wall on tokens[2];
			action = DarkForces::Message::DONE;
			value = std::stoi(tokens[1]);
			client = tokens[2];
		}
		else if (tokens[3] == "m_trigger") {
			action = DarkForces::Message::TRIGGER;
			value = std::stoi(tokens[1]);
			client = tokens[2];
		}
		break;
	case 3:
		// message: goto_stop 1
		if (tokens[1] == gotostop) {
			action = DarkForces::Message::GOTO_STOP;
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

void dfParseINF::parseSector(std::istringstream& infile, const std::string& sector)
{
	std::string line, dump;
	bool start = false;

	dfSector* pSector = m_level->findSector(sector);
	if (pSector == nullptr) {
		gaDebugLog(1, "dfParseINF::parseSector", "sector " + sector + "not in the level");
		return;
	}

	DarkForces::Component::InfStandardTrigger* program = nullptr;	// for a trigger standard
	DarkForces::Component::InfElevatorLight* light = nullptr;	// for elevator change_light
	DarkForces::Component::InfElevator* inv = nullptr;	// for elevator INV & MOVE_FLOOR

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
			if (program) {
				pSector->addProgram(program);
			}

			if (light && pSector) {
				pSector->addElevator(light);
			}

			if (inv && pSector) {
				// create a component mesh for the elevator and register on the sector
				dfMesh* mesh = inv->buildMesh();
				GameEngine::ComponentMesh* m_component = new GameEngine::ComponentMesh(mesh);

				pSector->addElevator(inv);
				pSector->addComponent(m_component, gaEntity::Flag::DELETE_AT_EXIT);

				// move the elevator at the default position at first run
				pSector->sendMessage(DarkForces::Message::GOTO_STOP_FORCE, 0);

				// else activate the default sounds
				newElevatorSound(inv, pSector);

				// if the eventMask of the sector is <> add a key manager
				if (inv->eventMask() != 0 || inv->key() != DarkForces::Keys::NONE) {
					DarkForces::Component::Trigger* trigger = new DarkForces::Component::Trigger();
					pSector->addComponent(trigger, gaEntity::Flag::DELETE_AT_EXIT);
				}
			}

			break;
		}
		else if (tokens[0] == "class:") {
			if (tokens[1] == "elevator") {
				if (tokens[2] == "change_light") {
					light = new DarkForces::Component::InfElevatorLight(pSector);
				}
				else if (tokens[2] == "inv") {
					inv = new DarkForces::Component::InfElevatorInv(pSector);
				}
				else if (tokens[2] == "move_floor") {
					inv = new DarkForces::Component::InfElevatorMoveFloor(pSector);
				}
				else if (tokens[2] == "basic") {
					inv = new DarkForces::Component::InfElevatorBasic(pSector);
				}
				else if (tokens[2] == "move_ceiling") {
					inv = new DarkForces::Component::InfElevatorMoveCeiling(pSector);
				}
				else if (tokens[2] == "morph_spin1") {
					inv = new DarkForces::Component::InfElevatorMorphSpin1(pSector);
				}
				else if (tokens[2] == "morph_spin2") {
					inv = new DarkForces::Component::InfElevatorMorphSpin2(pSector);
				}
				else if (tokens[2] == "morph_move1") {
					inv = new DarkForces::Component::InfElevatorMorphMove1(pSector);
				}
				else {
					// TODO remove onces all elevators are converted to components
					gaDebugLog(1, "dfParseINF::parseSector", "class " + tokens[1] + " not implemented");
				}
			}
			else if (tokens[1] == "trigger") {
				if (tokens.size() == 2) {
					std::cerr << "*class: trigger* not implemented" << std::endl;
				}
				else {
					program = new DarkForces::Component::InfStandardTrigger(tokens[2], sector);
				}
			}
		}
		else if (tokens[0] == "speed:") {
			if (inv) {
				inv->speed(std::stof(tokens[1]));
			}
			else if (light) {
				light->speed(std::stof(tokens[1]));
			}
		}
		else if (tokens[0] == "center:") {
			if (inv) {
				inv->center(-std::stof(tokens[1]), std::stof(tokens[2]));	//inverse X
			}
		}
		else if (tokens[0] == "angle:") {
			if (inv) {
				if (inv->type() == DarkForces::Component::InfElevator::Type::MORPH_MOVE1) {
					dynamic_cast<DarkForces::Component::InfElevatorHorizontal*>(inv)->angle(std::stof(tokens[1]));
				}
				else {
					gaDebugLog(1, "dfParseINF", "angle only implemented for morph_move1");
				}
			}
		}
		else if (tokens[0] == "event_mask:") {
			if (program) {
				program->eventMask(std::stoi(tokens[1]));
			}
			else if (light) {
				light->eventMask(std::stoi(tokens[1]));
			}
			else if (inv) {
				inv->eventMask(std::stoi(tokens[1]));
			}
		}
		else if (tokens[0] == "key:") {
			if (program) {
				gaDebugLog(1, "dfParseINF", "key not implemented for standard trigger");
			}
			else if (inv) {
				inv->key(tokens[1]);
			}
		}
		else if (tokens[0] == "client:") {
			if (program) {
				program->client(tokens[1]);
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
			else if (program) {
				program->message( parseMessage(tokens) );
			}
			else if (light || inv) {
				gaDebugLog(1, "dfParseINF::parseSector", "message: not implement for elevators/light/inv");
			}
		}
		else if (tokens[0] == "stop:") {
			stop = new dfLogicStop(pSector);

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

			if (light) {
				light->addStop(stop);
			}
			else if (inv) {
				inv->addStop(stop);
			}
		}
		else if (tokens[0] == "sound:") {
			if (light || inv) {
				uint32_t effect = std::stoi(tokens[1]);
				if (tokens[2] != "0") {
					addSound(pSector, effect - 1, tokens[2].c_str());
				}
				else {
					removeSound(pSector, effect - 1);
				}
			}
			else if (program) {
				gaDebugLog(1, "dfParseINF::parseSector", "sound: not implement for triggers");
			}
		}
		else {
			gaDebugLog(1, "dfParseINF::parseSector", tokens[0]+" not implement");
		}
	}
}

void dfParseINF::parseLine(std::istringstream& infile, const std::string &sector, int wallIndex)
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

dfParseINF::~dfParseINF(void)
{
	for (auto trigger : m_triggers) {
		delete trigger;
	}
}
