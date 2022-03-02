#include "dfFNT.h"

#include <sstream>
#include <fstream>
#include <iostream>

#include "../framework/fwTexture.h"
#include "dfFileSystem.h"
#include "dfPalette.h"

std::map<std::string, DarkForces::FNT*> g_dfFonts;

/**
 * load a font file
 */
DarkForces::FNT::FNT(const std::string& file)
{
	int size;
	char *m_data = g_dfFiles->load(DF_DARK_GOB, file, size);
	if (m_data == nullptr) {
		std::cerr << "FNT::FNT cannot load " << file << std::endl;
		return;
	}

	fntHeader* m_header = (fntHeader*)m_data;

	m_height = m_header->height;

	// compute the position of each character 
	size_t nbChars = m_header->last - m_header->first + 1;
	uint8_t* start = (uint8_t*) & (m_header->chars[0]);
	fntChar* dfChar = nullptr;

	for (size_t i = 0; i < nbChars; i++) {
		dfChar = (fntChar*)start;
		m_bitmaps[m_header->first + i] = dfChar;

		start += sizeof(uint8_t) + dfChar->width * m_height;
	}

	g_dfFonts[file] = this;
}

/**
 *
 */
void DarkForces::FNT::draw(fwTexture* texture, const std::string& text)
{
	texture->clear();	// clear the background

	int w, h, channels;
	uint8_t* data = texture->get_info(&w, &h, &channels);
	uint8_t c;
	fntChar* charSet=nullptr;
	uint32_t x = 0;
	uint8_t *bitmap;
	uint8_t* column=nullptr;
	glm::ivec4* rgba = nullptr;
	float a = 1.0f;

	data += w * (m_height + 2)* channels - channels;

	for (size_t i = 0; i < text.size(); i++) {
		c = text[i];

		// spaces are not encoded, 4 pixels space
		if (c == ' ') {
			data -= 4 * 4;
			continue;
		}

		// missing char
		if (m_bitmaps.count(c) == 0) {
			continue;
		}

		charSet = m_bitmaps[c];
		bitmap = charSet->bitmap;

		// char bitmaps are by columns
		// fwTexture adate are by rows
		for (size_t x1 = 0; x1 < charSet->width; x1++) {
			column = data;
			for (size_t y1 = 0; y1 < m_height; y1++) {
				rgba = g_dfCurrentPalette->getColor(*bitmap);

				// black = transparent
				if (*bitmap == 0) {
					a = 0.0;
				}
				else {
					a = 1.0f;
				}
				bitmap++;

				switch (channels) {
				case 4:
					column[0] = rgba->r;
					column[1] = rgba->g;
					column[2] = rgba->b;
					column[3] = a;

					column -= w * channels;	// move to next row of the fwTexture
					break;
				default:
					printf("DarkForces::FNT::draw bitmaps != RGBA not supported\n");
					__debugbreak();
				}
			}
			data -= channels;	// move to next row of the fwTexture
		}

		// 1 pixel space between chars
		data -= channels;
	}
}

/**
 *
 */
void DarkForces::FNT::draw(fwTexture* texture, const std::string& text, const std::string& font)
{
	g_dfFonts[font]->draw(texture, text);
}
