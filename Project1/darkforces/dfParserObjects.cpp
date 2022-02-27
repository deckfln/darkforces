#include "dfParserObjects.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../config.h"
#include "../framework/fwScene.h"

#include "../gaEngine/Lexer.h"
#include "../gaEngine/gaComponent/gaSound.h"

#include "lexer/dfObject.lex.h"

#include "dfFileSystem.h"
#include "dfParseINF.h"
#include "dfObject.h"
#include "dfModel/dfWAX.h"
#include "dfModel/dfFME.h"
#include "dfModel/df3DO.h"
#include "dfObject.h"
#include "dfPalette.h"
#include "dfAtlasTexture.h"
#include "dfGame.h"
#include "dfLevel.h"
#include "dfVOC.h"
#include "dfComponent/dfComponentLogic.h"
#include "dfComponent/dfEnemyAI.h"
#include "dfComponent/dfCSprite.h"
#include "dfComponent/dfCSprite/dfCSpriteAnimated.h"

#include "dfObject/dfObject3D/MouseBot.h"
#include "dfObject/Enemy.h"
#include "dfObject/df3DObject.h"
#include "dfObject/dfSpriteFME.h"
#include "dfObject/dfSpriteWAX.h"

#include "dfPlugin/dfSprites.h"

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
 * find the type of an object
 */
static uint32_t objectType(const GameEngine::ParserExpression& body)
{
	for (auto& component : body.m_children) {
		switch (component.m_children[0].m_expression) {
		case E_TYPE:
		case E_LOGIC: {
			const GameEngine::ParserExpression& logicType = component.m_children[0].m_children[2].m_children[0];
			switch (logicType.m_expression) {
			case O_PLAYER:
				return O_PLAYER;
			case O_PLANS:
				return O_PLANS;
			case E_ITEM: {
				const GameEngine::ParserExpression& item = component.m_children[0].m_children[2].m_children[0].m_children[0];
				switch (item.m_expression) {
				case E_SHIELD:
					return E_SHIELD;
				case E_ENERGY:
					return E_ENERGY;
				}
				break;
			}
			case O_SCENERY:
				return O_SCENERY;
			case O_BATTERY:
				return O_BATTERY;
			case O_STORM1:
				return O_STORM1;
			case O_OFFICERR:
			case I_OFFICER:
				return I_OFFICER;
			case O_MOUSEBOT:
				return O_MOUSEBOT;
			case O_INT_DROID:
				return O_INT_DROID;
			case O_MEDKIT:
				return O_MEDKIT;
			case O_COMMANDO:
				return O_COMMANDO;
			case O_SHIELD:
				return O_SHIELD;
			case O_TROOP:
				return O_TROOP;
			case O_SUPERCHARGE:
				return O_SUPERCHARGE;
			case O_LIFE:
				return O_LIFE;
			case O_KEY:
				return O_KEY;
			case O_GOGGLES:
				return O_GOGGLES;
			case O_RIFLE:
				return O_RIFLE;
			case O_REVIVE:
				return O_REVIVE;
			}
		}
		}
	}

	return -1;
}

/**
 * Parse all component of an object
 */
void dfParserObjects::parseObjectComponent(dfFileSystem* fs, DarkForces::Object* object, GameEngine::ParserExpression& component)
{
	switch (component.m_expression) {
	case E_TYPE:
	case E_LOGIC: {
		GameEngine::ParserExpression& logicType = component.m_children[2].m_children[0];
		switch (logicType.m_expression) {
		case O_PLAYER:
			object->hasCollider(true);
			break;
		case O_UPDATE:
			object->logic(dfLogic::ANIM);
			break;
		case O_PLANS:
			object->hasCollider(true);
			object->logic(dfLogic::PLANS);
			break;
		case E_ITEM: {
			GameEngine::ParserExpression& item = component.m_children[2].m_children[0].m_children[0];
			switch (item.m_expression) {
			case E_SHIELD:
				object->logic(dfLogic::ITEM_SHIELD);
				break;
			case E_ENERGY:
				object->logic(dfLogic::ITEM_ENERGY);
				break;
			}
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		}
		case O_ANIM:
			object->logic(dfLogic::ANIM);
			break;
		case O_SCENERY:
			object->logic(dfLogic::SCENERY);
			object->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::SCENERY_NORMAL);
			break;
		case O_BATTERY:
			object->logic(dfLogic::ITEM_BATTERY);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_STORM1:
			object->logic(dfLogic::TROOP | dfLogic::ANIM);
			object->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);
			object->hasCollider(true);
			break;
		case O_OFFICERR:
			object->logic(dfLogic::RED_KEY);
			// pass through
		case I_OFFICER:
			object->logic(dfLogic::OFFICER | dfLogic::ANIM);
			object->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);
			object->hasCollider(true);
			break;
		case O_INT_DROID:
			object->logic(dfLogic::INTDROID | dfLogic::ANIM);
			object->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);
			object->hasCollider(true);
			break;
		case O_MEDKIT:
			object->logic(dfLogic::MEDKIT);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_COMMANDO:
			object->logic(dfLogic::COMMANDO | dfLogic::ANIM);
			object->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);
			object->hasCollider(true);
			break;
		case O_SHIELD:
			object->logic(dfLogic::ITEM_SHIELD);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_TROOP:
			object->logic(dfLogic::TROOP | dfLogic::ANIM);
			object->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_STAY_STILL);
			object->hasCollider(true);
			break;
		case O_SUPERCHARGE:
			object->logic(dfLogic::ITEM_POWER);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_LIFE:
			object->logic(dfLogic::LIFE);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_KEY:
			object->logic(dfLogic::KEY_TRIGGER);
			object->logic(dfLogic::ANIM);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->hasCollider(true);
			object->gravity(false);
			//TODO : remove the hack
			break;
		case O_GOGGLES:
			object->logic(dfLogic::GOGGLES);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_RIFLE:
			object->logic(dfLogic::ITEM_RIFLE);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_REVIVE:
			object->logic(dfLogic::REVIVE);
			object->physical(false);	// objects can be traversed and are not subject to gravity
			object->gravity(false);
			object->hasCollider(true);
			break;
		case O_MOUSEBOT:
			object->logic(dfLogic::MOUSEBOT);
			object->hasCollider(true);
			break;
		}

		break; 
	}
	case E_EYE:
		break;
	case E_FLAGS:
		object->sendMessage(gaMessage::Action::ROTATE, gaMessage::Flag::ROTATE_AXE, component.m_children[2].m_token->m_vvalue);
		break;
	case E_D_YAW:
		object->sendMessage(gaMessage::Action::ROTATE, gaMessage::Flag::ROTATE_SPEED, component.m_children[2].m_token->m_vvalue);
		break;
	case E_RADIUS:
		object->radius(component.m_children[2].m_token->m_vvalue);
		break;
	case E_HEIGHT:
		object->height(component.m_children[2].m_token->m_vvalue);
		break;
	case E_PAUSE: {
		GameEngine::ParserExpression& truefalse = component.m_children[2].m_children[0];
		object->sendMessage(DarkForces::Message::ANIM_PAUSE, component.m_expression == O_TRUE);
		break;
	}
	case E_VUE: {
		gaMessage* msg = object->sendMessage(DarkForces::Message::ANIM_VUE);
		const std::string n = component.m_children[2].m_token->m_tvalue + "/" + '"' + component.m_children[3].m_children[1].m_token->m_tvalue + '"';
		strncpy_s(
			(char*)&msg->m_data,
			sizeof(msg->m_data),
			n.c_str(),
			_TRUNCATE);
		break; }
	}
}

/**
 * parse an object tree
 */
void dfParserObjects::parseObject(dfFileSystem* fs, GameEngine::ParserExpression& object, dfLevel* level, uint32_t objectID)
{
	switch (object.m_children[0].m_expression) {
	case E_CLASS: {
		GameEngine::ParserExpression& mclass = object.m_children[0].m_children[2].m_children[0];
		GameEngine::ParserExpression& attributes = object.m_children[1];
		GameEngine::ParserExpression& body = object.m_children[3];

		// extract attributes

		int data = (int)attributes.m_children[0].m_children[2].m_token->m_vvalue;
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

		int difficulty = (int)attributes.m_children[3].m_children[2].m_token->m_vvalue;

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
			ambient = sector->ambient() / 32.0f;
		}
		else {
#ifdef _DEBUG
			gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", "cannot find sector for " + std::to_string(position.x) + ":" + std::to_string(position.y) + " : " + std::to_string(position.z));
#endif
		}

		// create the object
		DarkForces::Object* obj=nullptr;
		uint32_t type = objectType(body);

		switch (mclass.m_expression) {
		case O_SPRITE: {
			dfWAX* wax = (dfWAX*)g_gaWorld.getModel(m_waxes[data]);

			switch (type) {
			case O_STORM1:
			case O_OFFICERR:
			case I_OFFICER:
			case O_INT_DROID:
			case O_COMMANDO:
			case O_TROOP:
				obj = new DarkForces::Enemy(wax, position, ambient, objectID);
				obj->sendMessage(DarkForces::Message::ROTATE, rotation);
				break;
			default:
				obj = new DarkForces::Sprite::WAX(m_waxes[data], position, ambient, objectID);
				obj->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::SCENERY_NORMAL);
			}
			obj->sendMessage(DarkForces::Message::ROTATE, rotation);
			break;
		}
		case O_3D:
			switch (type) {
			case O_MOUSEBOT:
				obj = new DarkForces::Anim::MouseBot(m_t3DOs[data], position, ambient, objectID);
				break;
			default:
				obj = new DarkForces::Anim::ThreeD(m_t3DOs[data], position, ambient, objectID);
			}
			break;
		
		case O_FRAME:
			obj = new DarkForces::Sprite::FME(m_fmes[data], position, ambient, objectID);
			break;
		
		default:
#ifdef _DEBUG
			gaDebugLog(LOW_DEBUG, "dfParserObjects::parseObject", "unknown code " + std::to_string(mclass.m_expression));
			return;
#endif
		}
		// record the object
		m_objects.push_back(obj);

		// add attributes
		obj->difficulty(difficulty);
		obj->sector(sector);

		// load all components of the object
		for (auto& component : body.m_children) {
			parseObjectComponent(fs, obj, component.m_children[0]);
		}

		// for enemies add an actor component and a sound component
		if (obj->isLogic(DF_LOGIC_ENEMIES)) {
			DarkForces::Enemy* enemy = dynamic_cast<DarkForces::Enemy*>(obj);
			enemy->setLevel(level);
		}

		// extend the object based on internal data
		obj->extend();
		break; 
	}
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

	m_level = level;

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
			m_t3DOs.resize((size_t)expression->m_children[1].m_token->m_vvalue);
			for (auto& expr : expression->m_children[2].m_children) {
				df3DO* threeDO = new df3DO(fs, palette, expr.m_children[2].m_token->m_tvalue);
				g_gaWorld.addModel(threeDO);
				m_t3DOs[current3DO++] = threeDO->name();
			}
			break; }

		case E_SPRS: {
			// load all WAX models
			m_waxes.resize((size_t)expression->m_children[1].m_token->m_vvalue);
			for (auto& expr : expression->m_children[2].m_children) {
				dfWAX* wax = new dfWAX(fs, palette, expr.m_children[2].m_token->m_tvalue);
				g_gaWorld.addModel(wax);
				m_waxes[currentWax++] = wax->name();
			}
			break; }

		case E_FMES: {
			// load all FME models
			m_fmes.resize((size_t)expression->m_children[1].m_token->m_vvalue);
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
			//m_objects.resize(expression->m_children[1].m_token->m_vvalue);
			uint32_t objectID = 0;
			for (auto& object : expression->m_children[2].m_children) {
				parseObject(fs, object, level, objectID);
				objectID++;
			}

			break; }

		default: {
			// unexpected code
			gaDebugLog(0, "dfParserObjects::parse", "unexpected code " + std::to_string(expression->m_expression));
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

	// load the 'pre-loaded' sprites
	std::list<GameEngine::Model*> l;
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

	std::list<GameEngine::Model*> l;
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
	g_dfSpritesEngine.init(m_objects.size(), m_textures);

	std::list<GameEngine::Model*> l;
	g_gaWorld.getModelsByClass(g_wax_class, l);

	for (auto wax : l) {
		g_dfSpritesEngine.addModel((dfWAX*)wax);
	}

	l.clear();
	g_gaWorld.getModelsByClass(g_fme_class, l);
	for (auto fme : l) {
		g_dfSpritesEngine.addModel((dfFME*)fme);
	}

	// add to the world
	for (auto object: m_objects) {
		if (object != nullptr) {
			g_gaWorld.addClient(object);
		}
	}

	time_t timer = GetTickCount64();
	g_dfSpritesEngine.update();
}

dfParserObjects::~dfParserObjects()
{
	for (auto object : m_objects) {
		delete object;
	}
}
