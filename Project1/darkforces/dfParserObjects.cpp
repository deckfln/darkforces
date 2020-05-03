#include "dfParserObjects.h"
#include <sstream>
#include <fstream>
#include <iostream>

#include "dfFileSystem.h"
#include "dfParseINF.h"
#include "dfWAX.h"
#include "dfObject/dfSprite.h"
#include "dfPalette.h"

dfObject* dfParserObjects::parseSprite(dfWAX* wax, float x, float y, float z, std::istringstream& infile)
{
	std::string line, dump;

	dfSprite* sprite = new dfSprite(wax, x, y, z);

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);

		if (tokens[0] == "SEQ") {
			// pass
		}
		else if (tokens[0] == "SEQEND") {
			break;
		}
		else if (tokens[0] == "LOGIC:") {
			if (tokens[1] == "SCENERY") {
				sprite->logic(DF_LOGIC_SCENERY);
			}
			else if (tokens[1] == "ANIM") {
				sprite->logic(DF_LOGIC_ANIM);
			}
		}
		else if (tokens[0] == "HEIGHT:") {
			sprite->height(std::stoi(tokens[1]));
		}
	}

	return sprite;
}

dfParserObjects::dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file)
{
	char* sec = fs->load(DF_DARK_GOB, file + ".O");
	std::istringstream infile(sec);
	std::string line, dump;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "SPRS") {
			m_waxes.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "SPR:") {
			m_waxes[m_currentWax++] = new dfWAX(fs, palette, tokens[1]);
		}
		else if (tokens[0] == "OBJECTS") {
			m_objects.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "CLASS:") {
			int data = std::stoi(tokens[3]);

			float x = std::stof(tokens[5]),
				y = std::stof(tokens[7]),
				z = std::stof(tokens[9]);

			if (tokens[1] == "SPRITE") {
				m_objects[m_currentObject++] = parseSprite(m_waxes[data], x, y, z, infile);
			}
		}

	}
}

dfParserObjects::~dfParserObjects()
{
	for (auto wax : m_waxes) {
		delete wax;
	}
	for (auto object : m_objects) {
		delete object;
	}
}
