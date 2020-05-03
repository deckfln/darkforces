#include "dfFME.h"

#include <iostream>
#include "dfPalette.h"

#pragma pack(push)
struct _FME_Header_from_WAX {
	long InsertX;	// Insertion point, X coordinate
					// Negative values shift the cell left
					// Positive values shift the cell right 
	long InsertY;	// Insertion point, Y coordinate
					// Negative values shift the cell up
					// Positive values shift the cell down 
	long Flip;		// 0 = not flipped
					// 1 = flipped horizontally 
	long Cell;		// pointer to CELL
					// = single picture 
	long UnitWidth;	// Unused 
	long UnitHeight;// Unused 
	long pad3;		// Unused 
	long pad4;		// Unuse
};

struct _FME_Image {
    long SizeX;     // Size of the FME, X value 
    long SizeY;     // Size of the FME, Y value 
    long Compressed;// 0 = not compressed
                    // 1 = compressed 
    long DataSize;  // Datasize for compressed FMEs,
                    // equals length of the FME file - 32
                    // If not compressed, DataSize = 0 
    long ColOffs;   // Always 0, because columns table 
                    // follows just after 
    long pad1;      // Unused
	unsigned char data[1];	// start of the data
};
#pragma pack(pop)

dfFME::dfFME(void* data, int offset, dfPalette* palette, bool from_wax)
{
	_FME_Header_from_WAX* header = (_FME_Header_from_WAX*)((char *)data + offset);
	m_InsertX = header->InsertX;
	m_InsertY = header->InsertY;

	_FME_Image* image = (_FME_Image*)((char *)data + header->Cell);

	m_width = image->SizeX;
	m_height = image->SizeY;
	m_nrChannels = 4;

	int size = m_width * m_height;
	m_data = new char[size * 4];
	int p, p1 = 0;
	glm::ivec4* rgb;
	unsigned char v;

	if (image->Compressed) {
		unsigned char* column;
		long* column_offset = (long*)image->data;
		int unpacked_bytes = 0;
		unsigned char control;
		for (auto i = 0; i < m_width; i++) {
			unpacked_bytes = 0;
			column = (unsigned char*)image + column_offset[i];
			p = 0;
			while (unpacked_bytes < m_height) {
				control = column[p++];
				if (control < 128) {
					for (int j = 0; j < control; j++) {
						v = column[p++];
						rgb = palette->getColor(v, true);

						m_data[p1] = rgb->r;
						m_data[p1 + 1] = rgb->g;
						m_data[p1 + 2] = rgb->b;
						m_data[p1 + 3] = rgb->a;
						p1 += 4;
					}
					unpacked_bytes += control;
				}
				else {
					for (int j = 0; j < control - 128; j++) {
						m_data[p1] = 0;
						m_data[p1 + 1] = 0;
						m_data[p1 + 2] = 0;
						m_data[p1 + 3] = 0;
						p1 += 4;
					}
					unpacked_bytes += (control - 128);
				}
			}
		}
	}
	else {
		// RAW images are stored by column
		// need to conver to  row first
		for (auto x = m_width - 1; x >= 0; x--) {
			for (auto y = m_height - 1; y >= 0; y--) {
				p = y * m_height + x;
				v = image->data[p];
				rgb = palette->getColor(v, true);

				m_data[p1] = rgb->r;
				m_data[p1 + 1] = rgb->g;
				m_data[p1 + 2] = rgb->b;
				m_data[p1 + 3] = rgb->a;
				p1 += 4;
			}
		}
	}

	if (header->Flip) {
		std::cerr << "dfFME::dfFME Flip not implemented" << std::endl;
	}
}

dfFME::~dfFME()
{
}
