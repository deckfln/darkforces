#include "dfSprites.h"

#include "../framework/fwMaterialBasic.h"
#include "../framework/fwScene.h"

static fwMaterialBasic* spriteMaterial=nullptr;

dfSprites::dfSprites(int nbSprites, dfAtlasTexture* atlas):
	fwSprites(nbSprites)
{
	m_positions.resize(nbSprites);
	m_directions.resize(nbSprites);
	m_textureIndex.resize(nbSprites);

	if (spriteMaterial == nullptr) {
		material = spriteMaterial = new fwMaterialBasic("data/shaders/sprites/sprites_vs.glsl", "", "data/shaders/sprites/sprites_fs.glsl");
	}

	set(&m_positions[0], atlas->texture(), 1000);
	geometry->addVertices("aTextureID", &m_textureIndex[0], 3, sizeof(glm::ivec3) * m_size, sizeof(long));

	atlas->bindToMaterial(material);
}

/**
 * Add a static sprite
 */
void dfSprites::add(glm::vec3 position, int textureID)
{
	m_positions[m_toDisplay] = position;
	m_textureIndex[m_toDisplay].r = textureID;

	m_toDisplay++;
	updated = true;
}

/**
 * Push updated attributes to the GPU
 */
void dfSprites::update(void)
{
	if (updated) {
		geometry->updateAttribute("aPos");
		geometry->updateAttribute("aTextureID");
		updated = false;
	}
}

void dfSprites::add2scene(fwScene* scene)
{
	if (!m_added) {
		m_added = true;
		scene->addChild(this);
	}
}

dfSprites::~dfSprites()
{
}
