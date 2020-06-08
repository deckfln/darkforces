#pragma once
#include <string>
#include "dfFileGOB.h"

enum {
	DF_DARK_GOB,
	DF_TEXTURES_GOB,
	DF_SPRITES_GOB,
	DF_SOUNDS_GOB
};

class dfFileSystem
{
	dfFileGOB* m_dark = nullptr;
	dfFileGOB* m_textures = nullptr;
	dfFileGOB* m_sprites = nullptr;
	dfFileGOB* m_sounds = nullptr;

public:
	dfFileSystem(std::string root);
	char* load(int mode, std::string file, int& size);
	~dfFileSystem();
};

extern dfFileSystem* dfFiles;