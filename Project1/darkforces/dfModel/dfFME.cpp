#include "dfFME.h"

#include <iostream>
#include <algorithm>

#include "../dfPalette.h"
#include "../dfFileSystem.h"
#include "../dfFrame.h"
#include "../../include/stb_image_write.h"

const float FME_WORLDSIZE_X = 95.0f;
const float FME_WORLDSIZE_Y = 95.0f;
/**
 * Create a FME from a file
 */
dfFME::dfFME(dfFileSystem* fs, dfPalette* palette, std::string& name):
	dfModel(name, false)
{
	int size;
	m_fromGOB = fs->load(DF_SPRITES_GOB, name, size);
	if (m_fromGOB == nullptr) {
		std::cerr << "dfFME::dfFME cannot load " << name << std::endl;
		return;
	}

	m_image = new dfFrame(m_fromGOB, 0, palette);

	// update the size of the opbject in level space;
	m_size_gl = glm::vec2(m_image->m_width / FME_WORLDSIZE_X, m_image->m_height / FME_WORLDSIZE_Y);
	m_insert_gl = glm::vec2(m_image->m_InsertX / FME_WORLDSIZE_X, (-m_image->m_InsertY - m_image->m_height) / FME_WORLDSIZE_Y);

	updateBoundingBox();

	delete m_fromGOB;
	m_fromGOB = nullptr;
}

/**
 * update the spriteModel based on the model
 */
void dfFME::spriteModel(GLmodel& model, int id)
{
	// level side of the sprite depend on texture size (pixel) / 32 * (Wwidth / 65536)

	SpriteModel* sm = &model.models[id];
	sm->size = m_size_gl;
	sm->insert = m_insert_gl;

	sm->world = glm::vec2(0.5, 1);
	sm->states = glm::i16vec2(0, 0);

	// Compress the WAX tables into flat tables
	sm->states.x = 65535;
	sm->states.y = m_image->m_textureID;

	dfModel::spriteModel(model, id);
}

dfFME::~dfFME()
{
	delete m_image;
}