#include "dfSprites.h"

#include "../framework/fwMaterialBasic.h"
#include "../framework/fwScene.h"

// data for sprite model
struct SpriteModel {
	glm::vec2 size;		// sprite size in pixel
	glm::vec2 insert;	// offset of center point
	glm::vec2 world;
	glm::vec2 textureID;
};

static fwMaterialBasic* spriteMaterial = nullptr;
static glUniformBuffer* models = nullptr;
static SpriteModel* modelsTable = nullptr;
static fwUniform* modelsUniform = nullptr;

dfSprites::dfSprites(int nbSprites, dfAtlasTexture* atlas):
	fwSprites(nbSprites)
{
	m_positions.resize(nbSprites);
	m_directions.resize(nbSprites);
	m_textureIndex.resize(nbSprites);

	if (spriteMaterial == nullptr) {
		std::map<ShaderType, std::string> shaders = {
			{GEOMETRY_SHADER, "data/shaders/sprites/sprites_gs.glsl"},
			{VERTEX_SHADER, "data/shaders/sprites/sprites_vs.glsl"},
			{FRAGMENT_SHADER_DEFERED, "data/shaders/sprites/sprites_fs.glsl"}
		};
		material = spriteMaterial = new fwMaterialBasic(shaders);

		models = new glUniformBuffer(sizeof(struct SpriteModel) * 32);
		modelsUniform = new fwUniform("Models", models);
		material->addUniform(modelsUniform);

		modelsTable = new SpriteModel[32];
		modelsTable[0].size = glm::vec2(18.0f / 32.0f, 46.0f / 64.0f);
		modelsTable[0].insert = glm::vec2(-8.0f / 32.0f, 0 / 64.0f);
		modelsTable[0].world = glm::vec2(0.5, 1);
		modelsTable[0].textureID.r = 0;
	}

	set(&m_positions[0], atlas->texture(), 1000);
	geometry->addAttribute("aData", GL_ARRAY_BUFFER, &m_textureIndex[0], 3, sizeof(glm::vec3) * m_size, sizeof(float), false);

	atlas->bindToMaterial(material);
}

/**
 * Add a static sprite
 */
void dfSprites::add(glm::vec3 position, int modelID, int textureID)
{
	m_positions[m_toDisplay] = position;
	m_textureIndex[m_toDisplay].r = textureID;
	m_textureIndex[m_toDisplay].g = textureID;

	m_toDisplay++;
	updated = true;
}

/**
 * Push updated attributes to the GPU
 */
void dfSprites::update(void)
{
	if (updated) {
		geometry->updateVertices(0, m_toDisplay);
		geometry->updateAttribute("aData", 0, m_toDisplay);
		//geometry->verticesToDisplay(m_toDisplay);
		models->bind();
		models->map(modelsTable, 0, sizeof(modelsTable) * 32);
		models->unbind();
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
