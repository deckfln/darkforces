#include "dfParserObjects.h"
#include <sstream>
#include <fstream>
#include <iostream>

#include "../framework/fwScene.h"

#include "dfFileSystem.h"
#include "dfParseINF.h"
#include "dfModel/dfWAX.h"
#include "dfModel/dfFME.h"
#include "dfModel/df3DO.h"
#include "dfObject.h"
#include "dfObject/dfSprite.h"
#include "dfObject/dfSpriteAnimated.h"
#include "dfObject/dfObject3D.h"
#include "dfPalette.h"
#include "dfAtlasTexture.h"
#include "dfSprites.h"
#include "dfGame.h"
#include "dfLevel.h"

dfObject* dfParserObjects::parseObject(dfObject* sprite, std::istringstream& infile)
{
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

		if (tokens[0] == "SEQ") {
			// pass
		}
		else if (tokens[0] == "SEQEND") {
			break;
		}
		else if (tokens[0] == "LOGIC:") {
			if (tokenMap["LOGIC:"] == "SCENERY") {
				sprite->logic(DF_LOGIC_SCENERY);
				((dfSpriteAnimated *)sprite)->state(DF_STATE_SCENERY_NORMAL);
			}
			else if (tokenMap["LOGIC:"] == "ANIM") {
				sprite->logic(DF_LOGIC_ANIM);
			}
			else if (tokenMap["LOGIC:"] == "STORM1") {
				sprite->logic(DF_LOGIC_TROOP | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["LOGIC:"] == "INT_DROID") {
				sprite->logic(DF_LOGIC_INTDROID | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["LOGIC:"] == "COMMANDO") {
				sprite->logic(DF_LOGIC_COMMANDO | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["LOGIC:"] == "I_OFFICER") {
				sprite->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["LOGIC:"] == "ITEM") {
				if (tokens[2] == "SHIELD") {
					sprite->logic(DF_LOGIC_ITEM_SHIELD);
				}
				else if (tokens[2] == "ENERGY") {
					sprite->logic(DF_LOGIC_ITEM_ENERGY);
				}
				else {
					std::cerr << "dfParserObjects::parseObject logic: item " << tokens[2] << " not implemented" << std::endl;
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
			else if (tokenMap["LOGIC:"] == "UPDATE") {
				sprite->logic(DF_LOGIC_ANIM);
			}
			else {
				std::cerr << "dfParserObjects::parseObject logic: " << tokenMap["LOGIC:"] << " not implemented" << std::endl;
			}
		}
		else if (tokens[0] == "TYPE:") {
			if (tokenMap["TYPE:"] == "I_OFFICER") {
				sprite->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["TYPE:"] == "I_OFFICERR") {
				sprite->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_RED_KEY | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["TYPE:"] == "COMMANDO") {
				sprite->logic(DF_LOGIC_COMMANDO | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["TYPE:"] == "TROOP") {
				sprite->logic(DF_LOGIC_TROOP | DF_LOGIC_ANIM);
				((dfSpriteAnimated*)sprite)->state(DF_STATE_ENEMY_STAY_STILL);
			}
			else if (tokenMap["TYPE:"] == "MOUSEBOT") {
				sprite->logic(DF_LOGIC_MOUSEBOT | DF_LOGIC_ANIM);
			}
			else {
				std::cerr << "dfParserObjects::parseObject type: " << tokenMap["TYPE:"] << " not implemented" << std::endl;
			}
		}
		else if (tokens[0] == "HEIGHT:") {
			sprite->height(std::stof(tokenMap["HEIGHT:"]));
		}
		else if (tokens[0] == "RADIUS:") {
			sprite->radius(std::stof(tokenMap["RADIUS:"]));
		}
		else if (tokens[0] == "FLAGS:") {
			((dfObject3D*)sprite)->animRotationAxe(std::stoi(tokenMap["FLAGS:"]));
		}
		else if (tokens[0] == "D_YAW:") {
			((dfObject3D*)sprite)->animRotationSpeed(std::stof(tokenMap["D_YAW:"]));
		}
		else if (tokens[0] == "D_PITCH:") {
			((dfObject3D*)sprite)->animRotationSpeed(std::stof(tokenMap["D_PITCH:"]));
		}
		else if (tokens[0] == "D_ROLL:") {
			((dfObject3D*)sprite)->animRotationSpeed(std::stof(tokenMap["D_ROLL:"]));
		}
		else {
			std::cerr << "dfParserObjects::parseObject command: " << tokens[0] << " not implemented" << std::endl;
		}
	}

	return sprite;
}

dfParserObjects::dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file, dfLevel *level)
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
		else if (tokens[0] == "FMES") {
			m_fmes.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "PODS") {
			m_3DOs.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "FME:") {
			m_fmes[m_currentFME++] = new dfFME(fs, palette, tokens[1]);
		}
		else if (tokens[0] == "SPR:") {
			m_waxes[m_currentWax++] = new dfWAX(fs, palette, tokens[1]);
		}
		else if (tokens[0] == "POD:") {
			if (tokens[1] == "MOUSEBOT.3DO" || tokens[1] == "DEATH.3DO") {
				m_3DOs[m_current3DO++] = new df3DO(fs, palette, tokens[1]);
			}
			else {
				m_3DOs[m_current3DO++] = nullptr;
			}
		}
		else if (tokens[0] == "OBJECTS") {
			m_objects.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "CLASS:") {
			int data = std::stoi(tokenMap["DATA:"]);

			glm::vec3 position(
				-std::stof(tokenMap["X:"]),
				std::stof(tokenMap["Z:"]),
				-std::stof(tokenMap["Y:"])
			);

			// convert level space to gl space and search the sectors
			float ambient = 0;
			dfSector* sector = level->findSectorLVL(position);
			if (sector) {
				ambient = sector->m_ambient / 32.0f;
			}
			else {
				std::cerr << "dfParserObjects::dfParserObjects cannot find sector for " << position.x << ":" << position.y << ":" << position.z << std::endl;
			}

			glm::vec3 rotation(std::stof(tokenMap["PCH:"]),
				std::stof(tokenMap["YAW:"]),
				std::stof(tokenMap["ROL:"])
			);

			int difficulty = std::stoi(tokenMap["DIFF:"]);

			if (tokenMap["CLASS:"] == "SPRITE") {
				dfSpriteAnimated* sprite = new dfSpriteAnimated(m_waxes[data], position, ambient);
				sprite->difficulty(difficulty);
				sprite->rotation(rotation);
				m_objects[m_currentObject] = parseObject(sprite, infile);

				m_currentObject++;
			}
			else if (tokenMap["CLASS:"] == "FRAME") {
				dfObject* frame = new dfSprite(m_fmes[data], position, ambient);
				frame->difficulty(difficulty);
				m_objects[m_currentObject] = parseObject(frame, infile);
				m_currentObject++;
			}
			else if (tokenMap["CLASS:"] == "3D") {
				dfObject3D* threedo = new dfObject3D(m_3DOs[data], position, ambient);
				threedo->difficulty(difficulty);
				m_objects[m_currentObject] = parseObject(threedo, infile);
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
	for (auto fme : m_fmes) {
		frames.push_back((dfBitmapImage *)fme->frame());
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

	for (auto fme : m_fmes) {
		m_sprites->addModel(fme);
	}

	dfObject* object;
	for (auto i = 0; i < m_currentObject; i++) {
		object = m_objects[i];

		if (object->is(OBJECT_3DO)) {
			continue;
		}

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
			m_sprites->add((dfSprite *)object);
		}
	}

	time_t timer = GetTickCount64();
	m_sprites->update();
}

void dfParserObjects::add2scene(fwScene* scene)
{
	if (!m_added) {
		m_added = true;
		m_sprites->set_name("dfSprites");
		m_sprites->add2scene(scene);

		for (auto object: m_objects) {
			if (object && object->is(OBJECT_3DO)) {
				((dfObject3D*)object)->add2scene(scene);
			}
		}
	}

	time_t timer = GetTickCount64();
	update(timer);
}

/**
 * Update all object on the level
 */
void dfParserObjects::update(time_t t)
{
	int i = 0;
	for (auto object : m_objects) {
		if (object && object->isLogic(DF_LOGIC_ANIM)) {
			object->update(t);
		}
	}

	m_sprites->update();
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
