#include "dfFME.h"

#include <iostream>
#include "../dfPalette.h"
#include "../dfFileSystem.h"
#include "../dfFrame.h"
#include "../../include/stb_image_write.h"

/**
 * Create a FME from a file
 */
dfFME::dfFME(dfFileSystem* fs, dfPalette* palette, std::string& name):
	dfModel(name)
{
	m_fromGOB = fs->load(DF_SPRITES_GOB, name);
	if (m_fromGOB == nullptr) {
		std::cerr << "dfFME::dfFME cannot load " << name << std::endl;
		return;
	}

	m_image = new dfFrame(m_fromGOB, 0, palette);

	delete m_fromGOB;
	m_fromGOB = nullptr;
}

/**
 * update the spriteModel based on the model
 */
void dfFME::spriteModel(GLmodel& model, int id)
{
	// level side of the sprite depend on texture size (pixel) / 32 * (Wwidth / 65536)
	float widthFactor = 58.0f;
	float heightFactor = 40.0f;

	SpriteModel* sm = &model.models[id];
	sm->size = glm::vec2(m_image->m_width / widthFactor, m_image->m_height / heightFactor);
	sm->insert = glm::vec2(m_image->m_InsertX / widthFactor, -m_image->m_InsertY / heightFactor);

	sm->world = glm::vec2(0.5, 1);
	sm->states = glm::i16vec2(0, 0);

	// Compress the WAX tables into flat tables
	sm->states.x = 65535;
	sm->states.y = m_image->m_textureID;

	m_id = id;
}

dfFME::~dfFME()
{
	delete m_image;
}