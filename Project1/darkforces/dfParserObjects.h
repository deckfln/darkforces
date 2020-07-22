#pragma once

#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <list>

#include "../gaEngine/Parser.h"

class dfFileSystem;
class dfSpriteAnimated;
class dfWAX;
class dfFME;
class dfObject;
class dfPalette;
class dfAtlasTexture;
class dfSprites;
class fwScene;
class dfLevel;
class df3DO;
class fwCylinder;

class gaCollisionPoint;

class dfParserObjects
{
	std::vector<dfObject *> m_objects;
	int m_currentObject = 0;
	bool m_added = false;

	std::vector<std::string> m_waxes;
	std::vector<std::string> m_fmes;
	std::vector<std::string> m_t3DOs;

	dfAtlasTexture* m_textures;
	dfObject* parseObject(dfFileSystem* fs, dfObject* sprite, std::istringstream& infile);
	void parseObject(dfFileSystem* fs, GameEngine::ParserExpression& object, dfLevel* level);
	void parseObjectComponent(dfFileSystem* fs, dfObject* object, GameEngine::ParserExpression& component);

public:
	dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file, dfLevel* level);
	dfAtlasTexture* buildAtlasTexture(void);
	void buildSprites(void);
	bool checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions);
	void parse(GameEngine::Parser& parser, dfFileSystem* fs, dfPalette* palette, dfLevel* level);
	~dfParserObjects();
};