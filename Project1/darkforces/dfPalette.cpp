#include "dfPalette.h"

#include <iostream>

dfPalette::dfPalette(dfFileGOB* gob, std::string file)
{
	// load a vga13h rgb palette
	m_palette = (dfPaletteColors *)gob->load(file);

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
		m_palette->colors[i] = vga13h;
	}
}

dfPaletteColor* dfPalette::getColor(int v)
{
	if (v < 0 || v > 255) {
		std::cerr << "dfPalette::getColor invalid index" << std::endl;
		return nullptr;
	}

	return &m_palette->colors[v];
}

dfPalette::~dfPalette()
{
	delete m_palette;
}
