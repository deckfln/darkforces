#include "dfFileLFD.h"
#include <map>
#include <vector>

#include "../config.h"
#include "../framework/fwTexture.h"

#pragma pack(push)
#pragma pack(1)

struct LFD_Entry
{
	char type[4];	 // type of the resource
	char name[8];	 // name of the resource
	uint32_t length;	// length of the resource
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
#pragma pack(pop)

/**
 *
 */
DarkForces::DELT::DELT(uint8_t* buffer, uint32_t size, std::vector<PLTT_RGB>* palette)
{
	DELT_Header* header = (DELT_Header*)buffer;
	DELT_Line* line;
	uint32_t p = sizeof(DELT_Header);
	uint16_t pixels;
	bool rle;

	// alocate a buffer to store all lines
	header->SizeX++;
	header->SizeY++;

	uint32_t width = header->SizeX - header->OffsX;
	uint32_t height = header->SizeY - header->OffsY;
	uint32_t rs = width * height;
	uint8_t* block = new uint8_t[rs * 4];
	uint32_t d;
	uint8_t colorIndex;

	while (p < size) {
		line = (DELT_Line*)(buffer + p);
		p += sizeof(DELT_Line);

		d = (line->StartY - header->OffsY) * width * 4 + (line->StartX - header->OffsX) * 4;

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

	// flipX
	uint32_t* flipx = new uint32_t[rs];
	uint32_t* flip = (uint32_t*)block;
	uint32_t* t,* s, *bmp = flipx;

	for (auto y = 0; y < height; y++) {
		t = flipx;
		s = flip + width - 1;
		for (auto x = 0; x < width; x++) {
			*(t++) = *(s--);
		}
		flipx += width;
		flip += width;
	}

	delete[] block;

	m_texture = new fwTexture((uint8_t*)bmp, width, height, 4, GL_NEAREST);
	m_posX = header->OffsX;
	m_posY = header->OffsY;
}

/**
 * load ANM files
 */
#pragma pack(push)
#pragma pack(1)
struct ANIM_DELTData
{
	uint32_t size;
	DELT_Header aDelt;
};

struct ANIM_Header
{
	uint16_t nbDelt;
};
#pragma pack(pop)

/**
 *
 */
DarkForces::ANIM::ANIM(uint8_t* buffer, std::vector<PLTT_RGB>* palette)
{
	struct ANIM_Header* header = (struct ANIM_Header*)buffer;
	struct ANIM_DELTData* data;

	buffer += sizeof(struct ANIM_Header);
	data = (ANIM_DELTData*)buffer;

	for (size_t i = 0; i < header->nbDelt; i++) {
		DELT* texture = new DELT((uint8_t*)&data->aDelt, data->size, palette);

		m_delts.push_back(texture);

		buffer += data->size + sizeof(uint32_t);
		data = (struct ANIM_DELTData*)buffer;
	}
}
/**
 * load PLTT data
 */
struct PLTT_Header
{
	uint8_t First;	// first color in the palette
	uint8_t Last;	// last color in the palette
	DarkForces::PLTT_RGB Color[1];	// n = Last - First + 1
};

void* DarkForces::FileLFD::parsePLTT(char* buffer, uint32_t size)
{
	std::vector<PLTT_RGB> *palette = new std::vector<PLTT_RGB>;
	palette->resize(255);

	PLTT_Header* header = (PLTT_Header*)buffer;

	for (auto i = 0; i < header->Last - header->First; i++) {
		palette->at(header->First + i) = header->Color[i];
	}

	return palette;
}

/**
 *
 */
DarkForces::FileLFD::FileLFD(const std::string& file)
{
	static char name[13];

	m_fd = std::ifstream(file, std::ios::in | std::ios::binary);
	if (!m_fd.is_open()) {
		std::cout << "failed to open " << file << '\n';
		__debugbreak();
	}

	// check magic an find master index
	LFD_Entry header;
	m_fd.read((char*)&header, sizeof(LFD_Entry));

	std::vector<LFD_Entry> index(header.length / sizeof(LFD_Entry));
	m_fd.read((char*)&index[0], header.length);

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
	/*
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
	*/
}

/**
 * load a permanent LFD
 */
static std::map<std::string, DarkForces::FileLFD*> g_LFDs;

void DarkForces::FileLFD::load(const std::string& file)
{
	g_LFDs[file] = new DarkForces::FileLFD(ROOT_FOLDER + "data/lfd/" + file + ".LFD");
}

DarkForces::ANIM* DarkForces::FileLFD::loadAnim(const std::string& file, const std::string& source)
{
	return g_LFDs[source]->anim(file);
}

/**
 * load specificaly an ANIM file
 */
DarkForces::ANIM* DarkForces::FileLFD::anim(const std::string& file)
{
	LFD_Entry entry;
	uint8_t* buffer;
	ANIM* anim;

	loadPltt("menu");
	m_fd.seekg(m_index["ANIM"+file].start);
	m_fd.read((char*)&entry, sizeof(LFD_Entry));
	buffer = new uint8_t[entry.length];
	m_fd.read((char*)buffer, entry.length);
	anim = new ANIM(buffer, (std::vector<PLTT_RGB> *)m_ressources["PLTTmenu"]);
	delete[] buffer;

	return anim;
}

/**
 * load specificaly an PLTT file
 */
void DarkForces::FileLFD::loadPltt(const std::string& file)
{
	LFD_Entry entry;
	uint8_t* buffer;

	std::string f = "PLTT" + file;
	if (m_index.count(f) == 0) {
		// try the default ones
		f = "PLTTbrf-jan";
	}
	m_fd.seekg(m_index[f].start);
	m_fd.read((char*)&entry, sizeof(LFD_Entry));
	buffer = new uint8_t[entry.length];
	m_fd.read((char*)buffer, entry.length);
	m_ressources["PLTT"+file] = parsePLTT((char*)buffer, entry.length);
	delete[] buffer;
}


/**
 *
 */
DarkForces::FileLFD::~FileLFD()
{
	for (auto& k : m_ressources) {
		delete k.second;
	}
	m_fd.close();
}

