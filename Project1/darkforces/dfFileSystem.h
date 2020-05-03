#pragma once
#include <string>
#include "dfFileGOB.h"

enum {
	DF_DARK_GOB,
	DF_TEXTURES_GOB,
	DF_SPRITES_GOB
};

class dfFileSystem
{
	dfFileGOB* m_dark = nullptr;
	dfFileGOB* m_textures = nullptr;
	dfFileGOB* m_sprites = nullptr;
public:
	dfFileSystem(std::string root);
	char* load(int mode, std::string file);
	~dfFileSystem();
};