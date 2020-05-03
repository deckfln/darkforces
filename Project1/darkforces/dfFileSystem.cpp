#include "dfFileSystem.h"

#include "config.h"

dfFileSystem::dfFileSystem(std::string root)
{
	m_dark = new dfFileGOB(root + "/data/dark.gob");
	m_textures = new dfFileGOB(root + "/data/textures.gob");
	m_sprites = new dfFileGOB(root + "/data/sprites.gob");
}

char* dfFileSystem::load(int mode, std::string file)
{
	switch (mode) {
	case DF_DARK_GOB:
		return m_dark->load(file);
	case DF_TEXTURES_GOB:
		return m_textures->load(file);
	case DF_SPRITES_GOB:
		return m_sprites->load(file);
	}

	return nullptr;
}

dfFileSystem::~dfFileSystem()
{
	delete m_dark;
	delete m_textures;
	delete m_sprites;
}
