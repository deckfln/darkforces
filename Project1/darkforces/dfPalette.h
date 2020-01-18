#pragma once

#include "dfFileGOB.h"

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
	dfPalette(dfFileGOB* gob, std::string file);
	dfPaletteColor* getColor(int v);
	~dfPalette();
};