#pragma once

#include "dfBitmap.h"

class dfPalette;

class dfFME: public dfBitmapImage
{
	long m_InsertX=0;	// Insertion point, X coordinate
						// Negative values shift the cell left
						// Positive values shift the cell right 
	long m_InsertY=0;	// Insertion point, Y coordinate
						// Negative values shift the cell up
						// Positive values shift the cell down 

	int m_references = 0;

public:
	dfFME(void *data, int offset, dfPalette* palette, bool from_wax=false);
	int references(int v) {
		m_references+=v;
		return m_references;
	};
	~dfFME();
};