#pragma once

#include <string>
#include <vector>

class dfFileSystem;
class dfSprite;
class dfWAX;
class dfObject;
class dfPalette;

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

	dfObject* parseSprite(dfWAX* wax, float x, float y, float z, std::istringstream& infile);

public:
	dfParserObjects(dfFileSystem* fs, dfPalette* palette, std::string file);
	~dfParserObjects();
};