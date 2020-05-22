#pragma once

#include "dfBitmap.h"

class dfPalette;

class dfFrame: public dfBitmapImage
{
	int m_references = 1;
public:
	long m_InsertX = 0;	// Insertion point, X coordinate
						// Negative values shift the cell left
						// Positive values shift the cell right 
	long m_InsertY = 0;	// Insertion point, Y coordinate
						// Negative values shift the cell up
						// Positive values shift the cell down 

	dfFrame(void* data, int offset, dfPalette* palette);
	int references(int v) {
		m_references += v;
		return m_references;
	};
	~dfFrame();
};