#include "dfBitmap.h"

#include <iostream>

#pragma pack(push)
struct dfBitmapHeader {
	char magic[4];		 // = 'BM ' + 0x1E 
	short SizeX;		// if = 1 then multiple BM in the file 
	short SizeY;		// EXCEPT if SizeY also = 1, in which case // it is a 1x1 BM 
	short idemX;		// unused by engine
	short idemY;		// unused by engine 
	char Transparent;	// 0x36 for normal, 0x3E for transparent, 0x08 for weapons 
	char logSizeY;		// logSizeY = log2(SizeY), logSizeY = 0 for weapons 
	short Compressed;	// 0 = not compressed, 1 = compressed (RLE), 2 = compressed (RLE0) 
	long DataSize;		// Data size for compressed BM // excluding header and columns starts table // If not compressed, DataSize is unused 
	char pad1[12];		// 12 times 0x00
};

struct dfBitMapHeaderSub {
	short SizeX;	// horizontal size 
	short SizeY;	// vertical size 
	short idemX;	// unused by engine 
	short idemY;	// unused by engine 
	long DataSize;	// unused (no compression allowed) 
	char logSizeY;	// logSizeY = log2(SizeY) 
	char pad1[3];
	char u1e[3];	 // these are always filled, but they seem // to be unused 
	char pad2[5];
	char Transparent; // 0x36 for normal // 0x3E for transparent pad3 byte[3] 
};

struct dfBitmapHeaderMultiple {
	char magic[4];	// = 'BM ' + 0x1E 
	short SizeX;	// = 1 
	short SizeY;	// = length of file - 32 
	short idemX;	// = -2 
	short idemY;	// number of 'sub' BMs 
	char Transparent;
	char logSizeY;
	short Compressed;
	long DataSize;
	char pad1[12]; // 12 times 0x00 
	char framerate;
	char pad;
	char index[1];	// offset of each sub bitmap
};
#pragma pack(pop)

static long ctol(char* bytes)
{
	return bytes[0] + bytes[1] * 256 + bytes[2] * 65536;
}

static bool _init = false;
static dfPaletteColors _color;

dfBitmap::dfBitmap(dfFileGOB* gob, std::string file, dfPalette* palette) :
	m_name(file)
{
	if (!_init) {
		for (auto i = 0; i < 256; i++) {

		}
	}

	if (file == "IWSECB3.BM") { // ZASWIT01.BM") {
		printf("dfBitmap::dfBitmap\n");
	}
	m_data = gob->load(file);
	if (m_data == nullptr) {
		std::cerr << "dfBitmap::dfBitmap cannot load "<< file << std::endl;
		return;
	}

	dfBitmapHeader* m_header = (dfBitmapHeader*)m_data;

	if (strncmp(m_header->magic, "BM \x1e", 3) != 0) {
		std::cerr << "dfBitmap::dfBitmap file " << file << " not a bitmap" << std::endl;
	}

	int height = m_header->SizeY;
	int width = m_header->SizeX;

	if (width == 1 && height != 1) {
		// multiple bitmaps
		m_multiple = true;
		m_nbImages = ((dfBitmapHeaderMultiple*)m_header)->idemY;
		dfBitmapHeaderMultiple* multipleHeader = (dfBitmapHeaderMultiple*)m_header;

		for (int i = 0; i < m_nbImages; i++) {
			// TODO find how to force the compiler to NOT align LONG
			long index = ctol(&multipleHeader->index[i*4]);
			dfBitMapHeaderSub* subImage = (dfBitMapHeaderSub*)((char *)m_header + sizeof(dfBitmapHeaderMultiple) - 2 + index);

			dfBitmapImage image;
			image.m_size = subImage->DataSize;
			image.m_height = subImage->SizeY;
			image.m_width = subImage->SizeX;
			image.m_transparent = (subImage->Transparent == '\x3e');
			image.m_raw = (char *)subImage + sizeof(dfBitMapHeaderSub);
			image.m_nrChannels = 3;
			image.m_data = convert2rgb(&image, palette);

			m_images.push_back(image);
		}
	}
	else {
		// single image
		dfBitmapImage image;
		image.m_size = m_header->DataSize;
		image.m_height = height;
		image.m_width = width;
		image.m_transparent = (m_header->Transparent == '\x3e');
		image.m_raw = (char*)m_header + sizeof(dfBitmapHeader);
		image.m_nrChannels = 3;
		image.m_data = convert2rgb(&image, palette);

		m_images.push_back(image);
	}
}

/**
 * Convert palette based to RGB
 */
char *dfBitmap::convert2rgb(dfBitmapImage *raw, dfPalette *palette)
{
	if (raw->m_transparent) {
		std::cerr << "dfBitmap::convert2rgb Transparency not implemented" << std::endl;
	}

	int size = raw->m_width * raw->m_height;
	char* image = new char[size * 3];
	int p, p1 = 0;
	dfPaletteColor *rgb;
	unsigned char v;

	// RAW images are stored by column
	// need to conver to  row first
	for (auto x = 0; x < raw->m_height; x++) {
		for (auto y = 0; y < raw->m_width; y++) {
			p = y * raw->m_height + x;
			v = raw->m_raw[p];
			rgb = palette->getColor(v);

			image[p1] = rgb->r;
			image[p1 + 1] = rgb->g;
			image[p1 + 2] = rgb->b;
			p1 += 3;
		}
	}

	return image;
}

/**
 * Return one image, by default image 0
 */
dfBitmapImage* dfBitmap::getImage(int index)
{
	if (m_images.size() > 0) {
		return &m_images[0];
	}

	return nullptr;
}

dfBitmap::~dfBitmap()
{
	free(m_data);

	for (auto& image : m_images) {
		delete[] image.m_data;
	}
}
