#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <list>

#include "dfObject.h"

#include "../gaEngine/Parser.h"

class dfFileSystem;
class dfSpriteAnimated;
class dfWAX;
class dfFME;
class dfPalette;
class dfAtlasTexture;
class Sprites;
class fwScene;
class dfLevel;
class df3DO;
class fwCylinder;

class gaCollisionPoint;

class dfParserObjects
{
	DarkForces::Object* p;

	std::vector<DarkForces::Object *> m_objects;
	int m_currentObject = 0;
	bool m_added = false;
	dfLevel* m_level = nullptr;

	std::vector<std::string> m_waxes;
	std::vector<std::string> m_fmes;
	std::vector<std::string> m_t3DOs;

	dfAtlasTexture* m_textures;
	void parseObject(dfFileSystem* fs, GameEngine::ParserExpression& object, dfLevel* level, uint32_t objectID);
	void parseObjectComponent(dfFileSystem* fs, DarkForces::Object* object, GameEngine::ParserExpression& component);

public:
	dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file, dfLevel* level);
	dfAtlasTexture* buildAtlasTexture(void);
	void buildSprites(void);
	void parse(GameEngine::Parser& parser, dfFileSystem* fs, dfPalette* palette, dfLevel* level);
	~dfParserObjects();
};