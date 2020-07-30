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
#include "dfComponent/dfComponentLogic.h"

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
			GameEngine::ParserExpression& item = component.m_children[2].m_children[0].m_children[0];
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
			object->logic(DF_LOGIC_MOUSEBOT);
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
			'"'+component.m_children[3].m_children[1].m_token->m_tvalue+'"'
		);
		break;
	}
}

/**
 * parse an object tree
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
		// record the object
		m_objects.push_back(obj);

		obj->difficulty(difficulty);

		// load all components of the object
		for (auto& component : body.m_children) {
			parseObjectComponent(fs, obj, component.m_children[0]);
		}

		// for enemies add an actor component
		if (obj->isLogic(DF_LOGIC_ENEMIES)) {
			obj->addComponent(new dfComponentActor());
		}

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
			//m_objects.resize(expression->m_children[1].m_token->m_vvalue);
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

dfParserObjects::~dfParserObjects()
{
	for (auto object : m_objects) {
		delete object;
	}
}
