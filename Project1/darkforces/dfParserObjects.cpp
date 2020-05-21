#include "dfParserObjects.h"
#include <sstream>
#include <fstream>
#include <iostream>

#include "dfFileSystem.h"
#include "dfParseINF.h"
#include "dfModel/dfWAX.h"
#include "dfObject/dfSprite.h"
#include "dfPalette.h"
#include "dfAtlasTexture.h"
#include "dfSprites.h"
#include "../framework/fwScene.h"
#include "dfGame.h"

dfObject* dfParserObjects::parseSprite(dfWAX* wax, float x, float y, float z, std::istringstream& infile)
{
	std::string line, dump;

	dfSprite* sprite = new dfSprite(wax, x, y, z);
	std::map<std::string, std::string> tokenMap;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);

		if (tokens[0] == "SEQ") {
			// pass
		}
		else if (tokens[0] == "SEQEND") {
			break;
		}
		else if (tokens[0] == "LOGIC:") {
			if (tokenMap["LOGIC:"] == "SCENERY") {
				sprite->logic(DF_LOGIC_SCENERY);
			}
			else if (tokenMap["LOGIC:"] == "ANIM") {
				sprite->logic(DF_LOGIC_ANIM);
			}
			else if (tokenMap["LOGIC:"] == "STORM1") {
				sprite->logic(DF_LOGIC_TROOP | DF_LOGIC_ANIM);
			}
			else if (tokenMap["LOGIC:"] == "INT_DROID") {
				sprite->logic(DF_LOGIC_INTDROID | DF_LOGIC_ANIM);
			}
			else if (tokenMap["LOGIC:"] == "COMMANDO") {
				sprite->logic(DF_LOGIC_COMMANDO | DF_LOGIC_ANIM);
			}
			else if (tokenMap["LOGIC:"] == "I_OFFICER") {
				sprite->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_ANIM);
			}
			else if (tokenMap["LOGIC:"] == "ITEM") {
				if (tokens[2] == "SHIELD") {
					sprite->logic(DF_LOGIC_ITEM_SHIELD);
				}
				else if (tokens[2] == "ENERGY") {
					sprite->logic(DF_LOGIC_ITEM_ENERGY);
				}
				else {
					std::cerr << "dfParserObjects::parseSprite logic: item " << tokens[2] << " not implemented" << std::endl;
				}
			}
			else if (tokenMap["LOGIC:"] == "SHIELD") {
				sprite->logic(DF_LOGIC_ITEM_SHIELD);
			}
			else if (tokenMap["LOGIC:"] == "LIFE") {
				sprite->logic(DF_LOGIC_LIFE);
			}
			else if (tokenMap["LOGIC:"] == "REVIVE") {
				sprite->logic(DF_LOGIC_REVIVE);
			}
			else {
				std::cerr << "dfParserObjects::parseSprite logic: " << tokenMap["LOGIC:"] << " not implemented" << std::endl;
			}
		}
		else if (tokens[0] == "TYPE:") {
			if (tokenMap["TYPE:"] == "I_OFFICER") {
				sprite->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_ANIM);
			}
			else if (tokenMap["TYPE:"] == "I_OFFICERR") {
				sprite->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_RED_KEY | DF_LOGIC_ANIM);
			}
			else if (tokenMap["TYPE:"] == "COMMANDO") {
				sprite->logic(DF_LOGIC_COMMANDO | DF_LOGIC_ANIM);
			}
			else if (tokenMap["TYPE:"] == "TROOP") {
				sprite->logic(DF_LOGIC_TROOP | DF_LOGIC_ANIM);
			}
			else {
				std::cerr << "dfParserObjects::parseSprite type: " << tokenMap["TYPE:"] << " not implemented" << std::endl;
			}
		}
		else if (tokens[0] == "HEIGHT:") {
			sprite->height(std::stof(tokens[1]));
		}
	}

	return sprite;
}

dfParserObjects::dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file)
{
	char* sec = fs->load(DF_DARK_GOB, file + ".O");
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
			int data = std::stoi(tokenMap["DATA:"]);

			float x = -std::stof(tokenMap["X:"]),
				y = std::stof(tokenMap["Z:"]),
				z = -std::stof(tokenMap["Y:"]);

			float pch = std::stof(tokenMap["PCH:"]),
				yaw = std::stof(tokenMap["YAW:"]),
				rol = std::stof(tokenMap["ROL:"]);

			int difficulty = std::stoi(tokenMap["DIFF:"]);

			if (tokenMap["CLASS:"] == "SPRITE") {
				m_objects[m_currentObject] = parseSprite(m_waxes[data], x, y, z, infile);
				m_objects[m_currentObject]->set(
					pch, yaw, rol, difficulty
				);

				m_currentObject++;
			}
			else {
				std::cerr << "dfParserObjects::parseSprite class: " << tokenMap["CLASS:"] << " not implemented" << std::endl;
			}
		}
	}
}

/**
 * Get all the FME and build an atlas texture
 */
dfAtlasTexture* dfParserObjects::buildAtlasTexture(void)
{
	std::vector<dfBitmapImage*> frames;
	for (auto wax : m_waxes) {
		wax->getFrames(frames);
	}

	m_textures = new dfAtlasTexture(frames);

	buildSprites();
	return m_textures;
}

/**
 * Create the sprites for the objects
 */
void dfParserObjects::buildSprites(void)
{
	// build the sprites
	m_sprites = new dfSprites(m_objects.size(), m_textures);

	for (auto wax : m_waxes) {
		m_sprites->addModel(wax);
	}

	dfObject* object;
	for (auto i = 0; i < m_currentObject; i++) {
		object = m_objects[i];

		/*
		DIFF	EASY	MED	HARD
		-3		X		X	X
		-2		X		X
		-1		X
		0		X		X	X
		1		X		X	X
		2				X	X
		3					X
		*/
		static bool difficultyTable[][3] = {
			{1, 1, 1},
			{1, 1, 0},
			{1, 0, 0},
			{1, 1, 1},
			{1, 1, 1},
			{0, 1, 1},
			{0, 0, 1}
		};

		int x = object->difficulty(), y = Game.difficulty();
		bool visible = difficultyTable[x][y];
		if (visible) {
			m_sprites->add(object);
		}
	}

	time_t timer = GetTickCount64();
	m_sprites->update(timer);
}

void dfParserObjects::add2scene(fwScene* scene)
{
	if (!m_added) {
		m_added = true;
		m_sprites->set_name("dfSprites");
		m_sprites->add2scene(scene);
	}

	time_t timer = GetTickCount64();
	update(timer);
}

/**
 * Update all object on the level
 */
void dfParserObjects::update(time_t t)
{
	m_sprites->update(t);
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
