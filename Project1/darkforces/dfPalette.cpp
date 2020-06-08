#include "dfPalette.h"

#include <iostream>

#include "dfFileSystem.h"

dfPalette::dfPalette(dfFileSystem* fs, std::string file)
{
	int size;

	// load a vga13h rgb palette
	m_palette = (dfPaletteColors *)fs->load(DF_DARK_GOB, file, size);

	if (m_palette == nullptr) {
		std::cerr << "dfPalette::dfPalette cannot load " << file << std::endl;
		exit(-1);
	}

	// and conver to a real rgb palette
	dfPaletteColor vga13h;
	for (auto i = 0; i < 256; i++) {
		vga13h = m_palette->colors[i];
		vga13h.r = (vga13h.r << 2) | (vga13h.r >> 4);
		vga13h.g = (vga13h.g << 2) | (vga13h.g >> 4);
		vga13h.b = (vga13h.b << 2) | (vga13h.b >> 4);
//		vga13h.a = 255;
		m_palette->colors[i] = vga13h;
	}
}

glm::ivec4* dfPalette::getColor(int v, bool transparent)
{
	static glm::ivec4 color;

	if (v < 0 || v > 255) {
		std::cerr << "dfPalette::getColor invalid index" << std::endl;
		return nullptr;
	}

	dfPaletteColor* rgb = &m_palette->colors[v];
	color.r = rgb->r;
	color.g = rgb->g;
	color.b = rgb->b;

	if (transparent) {
		color.a = (v != 0) * 255;	// if index==0 => alpha = 1.0
	}
	else {
		color.a = 255;
	}

	return &color;
}

dfPalette::~dfPalette()
{
	delete m_palette;
}
