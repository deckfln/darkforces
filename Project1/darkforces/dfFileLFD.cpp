#include "dfFileLFD.h"
#include <map>
#include <vector>

#include "../framework/fwTexture.h"

struct LFD_Entry
{
	char type[4];	 // type of the resource
	char name[8];	 // name of the resource
	uint32_t length;	// length of the resource
};

struct PLTT_RGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

/**
 * load DELT data
 */
struct DELT_Header
{
	int16_t OffsX; // X offset
	int16_t OffsY; // Y offset
	int16_t SizeX; // X size - 1 !
	int16_t SizeY; // Y size - 1 !
};

struct DELT_Line
{
	uint16_t SizeAndType; // size and compression of the line
	int16_t StartX;		// X position of line start
	int16_t StartY;		// Y position of line start
};

fwTexture* DarkForces::FileLFD::parseDELT(char* buffer, uint32_t size)
{
	DELT_Header* header = (DELT_Header*)buffer;
	DELT_Line* line;
	uint32_t p = sizeof(DELT_Header);
	uint16_t pixels;
	bool rle;
	
	// alocate a buffer to store all lines
	header->SizeX++;
	header->SizeY++;

	std::vector<PLTT_RGB>* palette = (std::vector<PLTT_RGB> *)m_ressources["PLTTbrf-jan"];

	uint32_t rs = header->SizeX * header->SizeY;
	std::vector<unsigned char> block(rs * 4);
	uint32_t d;
	uint8_t colorIndex;

	while (p < size) {
		line = (DELT_Line*)(buffer + p);
		p += sizeof(DELT_Line);

		d = line->StartY * header->SizeX * 4 + line->StartX * 4;

		pixels = line->SizeAndType >> 1;
		rle = (line->SizeAndType & 1);

		if (rle) {
			while (pixels > 0) {
				uint8_t b = buffer[p++];
				uint8_t bsize = b >> 1;

				if (b & 1) {
					// RLE block inside the RLE data
					colorIndex = buffer[p++];

					for (uint32_t i = 0; i < bsize; i++) {
						block[d++] = (*palette)[colorIndex].r;
						block[d++] = (*palette)[colorIndex].g;
						block[d++] = (*palette)[colorIndex].b;
						block[d++] = 255;
					}
				}
				else {
					// direct block inside the RLE data
					for (uint32_t i = 0; i < bsize; i++) {
						colorIndex = buffer[p++];
						block[d++] = (*palette)[colorIndex].r;
						block[d++] = (*palette)[colorIndex].g;
						block[d++] = (*palette)[colorIndex].b;
						block[d++] = 255;
					}
				}
				pixels -= bsize;
			}

		}
		else {
			for (uint32_t i = 0; i < pixels; i++) {
				colorIndex = buffer[p++];
				block[d++] = (*palette)[colorIndex].r;
				block[d++] = (*palette)[colorIndex].g;
				block[d++] = (*palette)[colorIndex].b;
				block[d++] = 255;
			}
		}
	}

	fwTexture* texture = new fwTexture(&block[0], header->SizeX, header->SizeY, 4, GL_NEAREST);

	return texture;
}


/**
 * load PLTT data
 */

struct PLTT_Header
{
	uint8_t First;	// first color in the palette
	uint8_t Last;	// last color in the palette
	PLTT_RGB Color[1];	// n = Last - First + 1
};

void* DarkForces::FileLFD::parsePLTT(char* buffer, uint32_t size)
{
	std::vector<PLTT_RGB> *palette = new std::vector<PLTT_RGB>;
	palette->resize(255);

	PLTT_Header* header = (PLTT_Header*)buffer;

	for (uint32_t i = 0; i < header->Last - header->First; i++) {
		(*palette)[header->First + i] = header->Color[i];
	}

	return palette;
}

/**
 *
 */
DarkForces::FileLFD::FileLFD(const std::string& file)
{
	static char name[13];

	fd = std::ifstream(file, std::ios::in | std::ios::binary);

	// check magic an find master index
	LFD_Entry header;
	fd.read((char*)&header, sizeof(LFD_Entry));

	std::vector<LFD_Entry> index(header.length / sizeof(LFD_Entry));
	fd.read((char*)&index[0], header.length);

	uint32_t p = sizeof(LFD_Entry) + header.length;

	for (auto& section : index) {
		memcpy(name, section.type, 4);
		memcpy(name + 4, section.name, 8);
		name[12] = 0;

		m_index[name].start = p;
		m_index[name].len = section.length;

		p += section.length + sizeof(LFD_Entry);
	}

	// load the palette first
	LFD_Entry entry;

	fd.seekg(m_index["PLTTbrf-jan"].start);
	fd.read((char*)&entry, sizeof(LFD_Entry));
	char* buffer = (char*)malloc(entry.length);
	fd.read(buffer, entry.length);
	m_ressources["PLTTbrf-jan"] = parsePLTT(buffer, entry.length);
	free(buffer);
	
	fd.seekg(m_index["DELTsecbase"].start);
	fd.read((char*)&entry, sizeof(LFD_Entry));
	buffer = (char*)malloc(entry.length);
	fd.read(buffer, entry.length);
	m_ressources["DELTsecbase"] = parseDELT(buffer, entry.length);
	free(buffer);
}

/**
 *
 */
DarkForces::FileLFD::~FileLFD()
{
	for (auto& k : m_ressources) {
		delete k.second;
	}
	fd.close();
}
