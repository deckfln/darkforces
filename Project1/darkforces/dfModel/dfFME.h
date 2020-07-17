#pragma once

#include "../dfModel.h"

class dfPalette;
class dfFileSystem;
class dfFrame;

// create runtime classes
extern const uint32_t g_fme_class;

class dfFME: public dfModel
{
	int m_references = 1;
	void* m_fromGOB = nullptr;
	dfFrame* m_image = nullptr;

public:
	dfFME(dfFileSystem* fs, dfPalette* palette, const std::string& name);
	dfFrame* frame(void) { return m_image; };
	void spriteModel(GLmodel& model, int id);
	~dfFME();
};