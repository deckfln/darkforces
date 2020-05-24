#pragma once

#include <string>
#include <vector>

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

class dfParserObjects
{
	int m_currentWax = 0;
	std::vector<dfWAX *> m_waxes;
	int m_currentFME = 0;
	std::vector<dfFME *> m_fmes;
	int m_current3DO = 0;
	std::vector<df3DO*> m_3DOs;

	std::vector<dfObject *> m_objects;
	int m_currentObject = 0;
	dfSprites* m_sprites = nullptr;
	bool m_added = false;

	dfAtlasTexture* m_textures;
	dfObject* parseObject(dfObject* sprite, std::istringstream& infile);

public:
	dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file, dfLevel* level);
	dfAtlasTexture* buildAtlasTexture(void);
	void buildSprites(void);
	void add2scene(fwScene*);
	void update(time_t t);
	~dfParserObjects();
};