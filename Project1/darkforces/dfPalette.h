#pragma once

#include <string>
#include <glm/vec4.hpp>

class dfFileSystem;

#pragma pack(push)
struct dfPaletteColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct dfPaletteColors {
	dfPaletteColor colors[256];
};
#pragma pack(pop)

class dfPalette
{
	dfPaletteColors *m_palette = nullptr;
public:
	dfPalette(dfFileSystem* fs, std::string file);
	glm::ivec4* getColor(int v, bool transparent = false);
	~dfPalette();
};

extern dfPalette* g_dfCurrentPalette;