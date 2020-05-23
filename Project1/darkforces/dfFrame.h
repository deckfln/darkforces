#pragma once

#include "dfBitmap.h"

class dfPalette;
class dfWAX;

class dfFrame: public dfBitmapImage
{
	int m_references = 1;
	dfWAX* m_parent = nullptr;	// if frame is part of a wax

public:
	long m_InsertX = 0;	// Insertion point, X coordinate
						// Negative values shift the cell left
						// Positive values shift the cell right 
	long m_InsertY = 0;	// Insertion point, Y coordinate
						// Negative values shift the cell up
						// Positive values shift the cell down 

	dfFrame(void* data, int offset, dfPalette* palette, dfWAX *parent = nullptr);
	int references(int v) {
		m_references += v;
		return m_references;
	};
	virtual void copyTo(unsigned char* target, int x, int y, int stride, int rgba, int Xcorner = 0, int Ycorner = 0);
	~dfFrame();
};