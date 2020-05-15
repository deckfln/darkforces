#include "dfSprites.h"

#include "../framework/fwMaterialBasic.h"
#include "../framework/fwScene.h"

static fwMaterialBasic* spriteMaterial = nullptr;
static glUniformBuffer* models = nullptr;
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

		m_models.resize(32);
	}

	set(&m_positions[0], atlas->texture(), 1000);
	geometry->addAttribute("aData", GL_ARRAY_BUFFER, &m_textureIndex[0], 3, sizeof(glm::vec3) * m_size, sizeof(float), false);

	atlas->bindToMaterial(material);
}

/**
 * Add a model (wax, fme)
 */
void dfSprites::addModel(dfModel* model)
{
	std::string& modelName = model->name();
	m_modelsIndex[modelName] = m_nbModels;

	SpriteModel* sm = &m_models[m_nbModels];
	model->spriteModel(sm);
	m_nbModels++;
}

/**
 * Add a static sprite
 */
void dfSprites::add(glm::vec3 position, std::string& modelName, int textureID)
{
	m_positions[m_toDisplay] = position;
	m_textureIndex[m_toDisplay].r = textureID;
	m_textureIndex[m_toDisplay].g = m_modelsIndex[modelName];

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
		models->map(&m_models[0], 0, m_models.size() * 32);
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
