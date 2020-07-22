#include "dfParserObjects.h"
#include <sstream>
#include <fstream>
#include <iostream>

#include "../config.h"
#include "../framework/fwScene.h"

#include "../gaEngine/Lexer.h"

#include "lexer/dfObject.lex.h"

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
static bool g_difficultyTable[][3] = {
	{1, 1, 1},
	{1, 1, 0},
	{1, 0, 0},
	{1, 1, 1},
	{1, 1, 1},
	{0, 1, 1},
	{0, 0, 1}
};

/**
 *
 */
dfObject* dfParserObjects::parseObject(dfFileSystem* fs, dfObject* sprite, std::istringstream& infile)
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
#ifdef DEBUG
					gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", " logic: item " + tokens[2] + " not implemented");
#endif
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
			else if (tokenMap["LOGIC:"] == "KEY") {
				sprite->logic(DF_LOGIC_KEY_TRIGGER);
				//TODO : remove the hack
				sprite->logic(DF_LOGIC_ANIM);
			}
			else {
#ifdef DEBUG
				gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", " logic: " + tokenMap["LOGIC:"] + " not implemented");
#endif // DEBUG
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
#ifdef DEBUG
				gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", " type: " + tokenMap["TYPE:"] + " not implemented");
#endif
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
		else if (tokens[0] == "PAUSE:") {
			((dfObject3D*)sprite)->pause(tokenMap["PAUSE:"] == "TRUE");
		}
		else if (tokens[0] == "VUE:") {
		((dfObject3D*)sprite)->vue(fs, tokens[1], tokens[2]);
		}
		else {
#ifdef DEBUG
		gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", " command: " + tokens[0] + " not implemented");
#endif
		}
	}

	// for enemies add an actor component
	if (sprite->isLogic(DF_LOGIC_ENEMIES)) {
		sprite->addComponent(new dfComponentActor());
	}
	return sprite;
}

/**
 * Parse all component of an object
 */
void dfParserObjects::parseObjectComponent(dfFileSystem* fs, dfObject* object, GameEngine::ParserExpression& component)
{
	switch (component.m_expression) {
	case E_TYPE:
	case E_LOGIC: {
		GameEngine::ParserExpression& logicType = component.m_children[2].m_children[0];
		switch (logicType.m_expression) {
		case O_PLAYER:
			break;
		case O_UPDATE:
			object->logic(DF_LOGIC_ANIM);
			break;
		case O_PLANS:
			break;
		case E_ITEM: {
			GameEngine::ParserExpression& item = component.m_children[2].m_children[0];
			switch (item.m_expression) {
			case E_SHIELD:
				object->logic(DF_LOGIC_ITEM_SHIELD);
				break;
			case E_ENERGY:
				object->logic(DF_LOGIC_ITEM_ENERGY);
				break;
			}
			break;
		}
		case O_ANIM:
			object->logic(DF_LOGIC_ANIM);
			break;
		case O_SCENERY:
			object->logic(DF_LOGIC_SCENERY);
			((dfSpriteAnimated*)object)->state(DF_STATE_SCENERY_NORMAL);
			break;
		case O_BATTERY:
			break;
		case O_STORM1:
			object->logic(DF_LOGIC_TROOP | DF_LOGIC_ANIM);
			((dfSpriteAnimated*)object)->state(DF_STATE_ENEMY_STAY_STILL);
			break;
		case O_OFFICERR:
			object->logic(DF_LOGIC_RED_KEY);
			// pass through
		case I_OFFICER:
			object->logic(DF_LOGIC_I_OFFICER | DF_LOGIC_ANIM);
			((dfSpriteAnimated*)object)->state(DF_STATE_ENEMY_STAY_STILL);
			break;
		case O_MOUSEBOT:
			object->logic(DF_LOGIC_MOUSEBOT | DF_LOGIC_ANIM);
			break;
		case O_INT_DROID:
			object->logic(DF_LOGIC_INTDROID | DF_LOGIC_ANIM);
			((dfSpriteAnimated*)object)->state(DF_STATE_ENEMY_STAY_STILL);
			break;
		case O_MEDKIT:
			break;
		case O_COMMANDO:
			object->logic(DF_LOGIC_COMMANDO | DF_LOGIC_ANIM);
			((dfSpriteAnimated*)object)->state(DF_STATE_ENEMY_STAY_STILL);
			break;
		case O_SHIELD:
			break;
		case O_TROOP:
			object->logic(DF_LOGIC_TROOP | DF_LOGIC_ANIM);
			((dfSpriteAnimated*)object)->state(DF_STATE_ENEMY_STAY_STILL);
			break;
		case O_SUPERCHARGE:
			break;
		case O_LIFE:
			object->logic(DF_LOGIC_LIFE);
			break;
		case O_KEY:
			object->logic(DF_LOGIC_KEY_TRIGGER);
			//TODO : remove the hack
			object->logic(DF_LOGIC_ANIM);
			break;
		case O_GOGGLES:
			break;
		case O_RIFLE:
			break;
		case O_REVIVE:
			object->logic(DF_LOGIC_REVIVE);
			break;
		}
		break; 
	}
	case E_EYE:
		break;
	case E_FLAGS:
		((dfObject3D*)object)->animRotationAxe(component.m_children[2].m_token->m_vvalue);
		break;
	case E_D_YAW:
		((dfObject3D*)object)->animRotationSpeed(component.m_children[2].m_token->m_vvalue);
		break;
	case E_RADIUS:
		object->radius(component.m_children[2].m_token->m_vvalue);
		break;
	case E_HEIGHT:
		object->height(component.m_children[2].m_token->m_vvalue);
		break;
	case E_PAUSE: {
		GameEngine::ParserExpression& truefalse = component.m_children[2].m_children[0];
		((dfObject3D*)object)->pause(component.m_expression == O_TRUE);
		break;
	}
	case E_VUE:
		((dfObject3D*)object)->vue(fs,
			component.m_children[2].m_token->m_tvalue,
			component.m_children[3].m_children[0].m_token->m_tvalue
		);
		break;
	}
}

/**
 * parse an obbject tree
 */
void dfParserObjects::parseObject(dfFileSystem* fs, GameEngine::ParserExpression& object, dfLevel* level)
{
	switch (object.m_children[0].m_expression) {
	case E_CLASS: {
		GameEngine::ParserExpression& mclass = object.m_children[0].m_children[2].m_children[0];
		GameEngine::ParserExpression& attributes = object.m_children[1];
		GameEngine::ParserExpression& body = object.m_children[3];

		// extract attributes

		int data = attributes.m_children[0].m_children[2].m_token->m_vvalue;
		GameEngine::ParserExpression& e_position = attributes.m_children[1];
		glm::vec3 position(
			-e_position.m_children[0].m_children[2].m_token->m_vvalue,
			e_position.m_children[2].m_children[2].m_token->m_vvalue,
			-e_position.m_children[1].m_children[2].m_token->m_vvalue
		);
		GameEngine::ParserExpression& e_rotation = attributes.m_children[2];
		glm::vec3 rotation(
			e_rotation.m_children[0].m_children[2].m_token->m_vvalue,
			e_rotation.m_children[1].m_children[2].m_token->m_vvalue,
			e_rotation.m_children[2].m_children[2].m_token->m_vvalue
		);

		int difficulty = attributes.m_children[3].m_children[2].m_token->m_vvalue;

		// ignore objects of higher difficulty
		int y = Game.difficulty();
		bool visible = g_difficultyTable[difficulty + 3][y];
		if (!visible) {
			return;
		}

		// find the sprite lightning
		// convert level space to gl space and search the sectors
		float ambient = 0;
		dfSector* sector = level->findSectorLVL(position);
		if (sector) {
			ambient = sector->m_ambient / 32.0f;
		}
		else {
#ifdef DEBUG
			gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", "cannot find sector for " + std::to_string(position.x) + ":" + std::to_string(position.y) + " : " + std::to_string(position.z));
#endif
		}

		// create the object
		dfObject* obj=nullptr;

		switch (mclass.m_expression) {
		case O_SPRITE: {
			dfWAX* wax = (dfWAX*)g_gaWorld.getModel(m_waxes[data]);
			obj = new dfSpriteAnimated(wax, position, ambient);
			((dfSpriteAnimated*)obj)->rotation(rotation);
			break;
		}
		case O_3D: {
			df3DO* tdo = (df3DO*)g_gaWorld.getModel(m_t3DOs[data]);
			obj = new dfObject3D(tdo, position, ambient);
			break;
		}
		case O_FRAME: {
			dfFME* fme = (dfFME*)g_gaWorld.getModel(m_fmes[data]);
			obj = new dfSprite(fme, position, ambient);
			break;
		}
		default:
#ifdef DEBUG
			gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", "unknown code " + std::to_string(mclass.m_expression));
			return;
#endif
		}
		obj->difficulty(difficulty);

		// load all components of the object
		for (auto& component : body.m_children) {
			parseObjectComponent(fs, obj, component.m_children[0]);
		}
		break; }
	case O_CLASS:
		break;
	}
}

/**
 * use the parsed tree
 */
void dfParserObjects::parse(GameEngine::Parser& parser, dfFileSystem* fs, dfPalette* palette, dfLevel* level)
{
	GameEngine::ParserExpression* expression;
	GameEngine::ParserExpression* sub_expression;

	int currentWax = 0;
	int currentFME = 0;
	int current3DO = 0;

	while (expression = parser.next()) {
		switch (expression->m_expression) {
		case E_MAGIC:
			if (expression->m_children[1].m_token->m_vvalue != 1.1) {
				gaDebugLog(0, "dfParserObjects::parse", "invalidmagic version");
			}
			break;

		case E_LEVELNAME:
			break;

		case E_PODS: {
			// load all 3DO models
			m_t3DOs.resize(expression->m_children[1].m_token->m_vvalue);
			for (auto& expr : expression->m_children[2].m_children) {
				df3DO* threeDO = new df3DO(fs, palette, expr.m_children[2].m_token->m_tvalue);
				g_gaWorld.addModel(threeDO);
				m_t3DOs[current3DO++] = threeDO->name();
			}
			break; }

		case E_SPRS: {
			// load all WAX models
			m_waxes.resize(expression->m_children[1].m_token->m_vvalue);
			for (auto& expr : expression->m_children[2].m_children) {
				dfWAX* wax = new dfWAX(fs, palette, expr.m_children[2].m_token->m_tvalue);
				g_gaWorld.addModel(wax);
				m_waxes[currentWax++] = wax->name();
			}
			break; }

		case E_FMES: {
			// load all FME models
			m_fmes.resize(expression->m_children[1].m_token->m_vvalue);
			for (auto& expr : expression->m_children[2].m_children) {
				dfFME* fme = new dfFME(fs, palette, expr.m_children[2].m_token->m_tvalue);
				g_gaWorld.addModel(fme);
				m_fmes[currentFME++] = fme->name();
			}
			break; }

		case E_SOUNDS:
			break;

		case E_OBJECTS: {
			// load all objcts
			m_objects.resize(expression->m_children[1].m_token->m_vvalue);
			for (auto& object : expression->m_children[2].m_children) {
				parseObject(fs, object, level);
			}

			break; }

		default: {
			// unexpected code
			gaDebugLog(0, "dfParserObjects::parse", "unexpeted code " + std::to_string(expression->m_expression));
		}
		}
	}
}

/**
 * Load the .O file
 */
dfParserObjects::dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file, dfLevel *level)
{
	int size;
	char* sec = fs->load(DF_DARK_GOB, file + ".O", size);
	std::istringstream infile(sec);
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;

	GameEngine::Lexer lexer(std::string(sec), g_keywords);
	GameEngine::Parser parser(lexer, g_dfObjectParse, g_keywords);
	parse(parser, fs, palette, level);
	/*
	int currentWax = 0;
	int currentFME = 0;
	int current3DO = 0;

	std::vector<std::string> waxes;
	std::vector<std::string> fmes;
	std::vector<std::string> t3DOs;

	const struct GameEngine::token* token;

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
			waxes.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "FMES") {
			fmes.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "PODS") {
			t3DOs.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "FME:") {
			dfFME* fme = new dfFME(fs, palette, tokens[1]);
			g_gaWorld.addModel(fme);
			fmes[currentFME++] = fme->name();
		}
		else if (tokens[0] == "SPR:") {
			dfWAX* wax = new dfWAX(fs, palette, tokens[1]);
			g_gaWorld.addModel(wax);
			waxes[currentWax++] = wax->name();
		}
		else if (tokens[0] == "POD:") {
			df3DO* threeDO = new df3DO(fs, palette, tokens[1]);
			g_gaWorld.addModel(threeDO);
			t3DOs[current3DO++] = threeDO->name();
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
#ifdef DEBUG
				gaDebugLog(LOW_DEBUG, "dfParserObjects::dfParserObjects", "cannot find sector for "+ std::to_string(position.x) + ":" + std::to_string(position.y) + " : " + std::to_string(position.z));
#endif
			}

			glm::vec3 rotation(std::stof(tokenMap["PCH:"]),
				std::stof(tokenMap["YAW:"]),
				std::stof(tokenMap["ROL:"])
			);

			int difficulty = std::stoi(tokenMap["DIFF:"]);


			if (tokenMap["CLASS:"] == "SPRITE") {
				dfWAX* wax = (dfWAX*)g_gaWorld.getModel(waxes[data]);
				dfSpriteAnimated* sprite = new dfSpriteAnimated(wax, position, ambient);
				sprite->difficulty(difficulty);
				sprite->rotation(rotation);
				m_objects[m_currentObject++] = parseObject(fs, sprite, infile);
			}
			else if (tokenMap["CLASS:"] == "FRAME") {
				dfFME* fme = (dfFME*)g_gaWorld.getModel(fmes[data]);
				dfObject* frame = new dfSprite(fme, position, ambient);
				frame->difficulty(difficulty);
				m_objects[m_currentObject++] = parseObject(fs, frame, infile);
			}
			else if (tokenMap["CLASS:"] == "3D") {
				df3DO* tdo = (df3DO*)g_gaWorld.getModel(t3DOs[data]);
				dfObject3D* threedo = new dfObject3D(tdo, position, ambient);
				threedo->difficulty(difficulty);
				m_objects[m_currentObject++] = parseObject(fs, threedo, infile);
			}
			else {
#ifdef DEBUG
				gaDebugLog(LOW_DEBUG, "dfParserObjects::parseSprite", "class : " + tokenMap["CLASS : "] + " not implemented");
#endif
			}
		}
	}
	*/

	// load the 'pre-loaded' sprites
	std::list<GameEngine::gaModel*> l;
	g_gaWorld.getModelsByClass(GameEngine::PRELOAD, l);
	for (auto model : l) {
		g_gaWorld.removeModel(model->name());

		// replace the place holder with the real model
		if (model->name().find(".WAX") != std::string::npos) {
			dfWAX* wax = new dfWAX(fs, palette, model->name());
			g_gaWorld.addModel(wax);
		}
		else if (model->name().find(".FME") != std::string::npos) {
			dfFME* fme = new dfFME(fs, palette, model->name());
			g_gaWorld.addModel(fme);
		}
	}

	delete sec;
}

/**
 * Get all the FME and build an atlas texture
 */
dfAtlasTexture* dfParserObjects::buildAtlasTexture(void)
{
	std::vector<dfBitmapImage*> frames;

	std::list<GameEngine::gaModel*> l;
	g_gaWorld.getModelsByClass(g_wax_class, l);
	for (auto wax : l) {
		((dfWAX*)wax)->getFrames(frames);
	}

	l.clear();
	g_gaWorld.getModelsByClass(g_fme_class, l);
	for (auto fme : l) {
		frames.push_back((dfBitmapImage *)((dfFME*)fme)->frame());
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
	dfSprites* manager = new dfSprites(m_objects.size(), m_textures);
	g_gaWorld.spritesManager(manager);

	std::list<GameEngine::gaModel*> l;
	g_gaWorld.getModelsByClass(g_wax_class, l);

	for (auto wax : l) {
		manager->addModel((dfWAX*)wax);
	}

	l.clear();
	g_gaWorld.getModelsByClass(g_fme_class, l);
	for (auto fme : l) {
		manager->addModel((dfFME*)fme);
	}

	// add to the world
	for (auto object: m_objects) {
		if (object != nullptr) {
			g_gaWorld.addClient(object);
		}
	}

	time_t timer = GetTickCount64();
	manager->update();
}

/**
 * test the objects for a collision
 */
bool dfParserObjects::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions)
{
	bool intersect = false;
	for (auto object : m_objects) {
		if (object != nullptr and object->checkCollision(bounding, direction, intersection, collisions)) {
			intersect = true;
		}
	}
	return intersect;
}

dfParserObjects::~dfParserObjects()
{
	for (auto object : m_objects) {
		delete object;
	}
}
