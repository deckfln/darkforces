#include "dfFileSystem.h"

#include "config.h"

dfFileSystem* g_dfFiles = nullptr;

dfFileSystem::dfFileSystem(std::string root)
{
	m_dark = new dfFileGOB(root + "/data/dark.gob");
	m_textures = new dfFileGOB(root + "/data/textures.gob");
	m_sprites = new dfFileGOB(root + "/data/sprites.gob");
	m_sounds = new dfFileGOB(root + "/data/sounds.gob");

	if (g_dfFiles == nullptr) {
		g_dfFiles = this;
	}
}

char* dfFileSystem::load(int mode, std::string file, int& size)
{
	switch (mode) {
	case DF_DARK_GOB:
		return m_dark->load(file, size);
	case DF_TEXTURES_GOB:
		return m_textures->load(file, size);
	case DF_SPRITES_GOB:
		return m_sprites->load(file, size);
	case DF_SOUNDS_GOB:
		return m_sounds->load(file, size);
	}

	return nullptr;
}

dfFileSystem::~dfFileSystem()
{
	delete m_dark;
	delete m_textures;
	delete m_sprites;
}
