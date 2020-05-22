#pragma once

#include "../dfModel.h"

class dfPalette;
class dfFileSystem;
class dfFrame;

class dfFME: public dfModel
{
	int m_references = 1;
	void* m_fromGOB = nullptr;
	dfFrame* m_image = nullptr;

public:
	dfFME(dfFileSystem* fs, dfPalette* palette, std::string& name);
	dfFrame* frame(void) { return m_image; };
	void spriteModel(GLmodel& model, int id);
	~dfFME();
};