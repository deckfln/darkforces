#pragma once

#include <string>
#include <vector>

class dfFileSystem;
class dfSprite;
class dfWAX;
class dfObject;
class dfPalette;
class dfAtlasTexture;
class dfSprites;
class fwScene;

enum {
	DF_LOGIC_SCENERY = 1,
	DF_LOGIC_ANIM = 2
};

class dfParserObjects
{
	std::vector<dfWAX *> m_waxes;
	int m_currentWax = 0;

	std::vector<dfObject *> m_objects;
	int m_currentObject = 0;
	dfSprites* m_sprites = nullptr;
	bool m_added = false;

	dfAtlasTexture* m_textures;
	dfObject* parseSprite(dfWAX* wax, float x, float y, float z, std::istringstream& infile);

public:
	dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file);
	dfAtlasTexture* buildAtlasTexture(void);
	void buildSprites(void);
	void add2scene(fwScene*);
	~dfParserObjects();
};