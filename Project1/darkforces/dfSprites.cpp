#include "dfSprites.h"

#include "../framework/fwMaterialBasic.h"
#include "../framework/fwScene.h"

#include "../gaEngine/gaBoundingBoxes.h"

#include "dfObject/dfSpriteAnimated.h"

static fwMaterialBasic* spriteMaterial = nullptr;
static glUniformBuffer* models = nullptr;
static fwUniform* modelsUniform = nullptr;

dfSprites::dfSprites(int nbSprites, dfAtlasTexture* atlas):
	fwSprites(nbSprites)
{
	set_name("dfSprites");

	m_positions.resize(nbSprites);
	m_directions.resize(nbSprites);
	m_textureIndex.resize(nbSprites);

	if (spriteMaterial == nullptr) {
		std::map<ShaderType, std::string> shaders = {
			{GEOMETRY_SHADER, "data/shaders/sprites/sprites_gs.glsl"},
			{VERTEX_SHADER, "data/shaders/sprites/sprites_vs.glsl"},
			{FRAGMENT_SHADER_DEFERED, "data/shaders/sprites/sprites_fs.glsl"}
		};
		m_material = spriteMaterial = new fwMaterialBasic(shaders);

		models = new glUniformBuffer(sizeof(struct GLmodel));
		modelsUniform = new fwUniform("Models", models);
		m_material->addUniform(modelsUniform);
	}

	set(&m_positions[0], atlas->texture(), 1000);
	m_geometry->addAttribute("aData", GL_ARRAY_BUFFER, &m_textureIndex[0], 4, sizeof(glm::vec4) * m_size, sizeof(float), false);
	m_geometry->addAttribute("aDirection", GL_ARRAY_BUFFER, &m_directions[0], 3, sizeof(glm::vec3) * m_size, sizeof(float), false);

	atlas->bindToMaterial(m_material);
}

/**
 * Add a model (wax, fme)
 */
void dfSprites::addModel(dfModel* model)
{
	const std::string& modelName = model->name();
	m_modelsIndex[modelName] = m_nbModels;

	model->spriteModel(m_models, m_nbModels);
	m_nbModels++;

	m_dirtyModels = true;
}

/**
 * Add a static sprite
 */
void dfSprites::add(dfSprite *object)
{
	// find an empty slot
	size_t slot;

	if (m_freeList.size() == 0) {
		slot = m_objects.size();
		m_objects.resize(m_objects.size() + 1);
		m_textureIndex.resize(m_objects.size() + 1);
		m_nbObjects++;
		m_toDisplay++;
	}
	else {
		slot = m_freeList.front();
		m_freeList.pop();
	}

	m_objects[slot] = object;
	m_textureIndex[slot].g = (float)m_modelsIndex[object->model()];

	object->updateSprite(
		&m_positions[slot],
		&m_textureIndex[slot],
		&m_directions[slot]
	);

	m_updated = true;
}

/**
 * Push updated attributes to the GPU
 */
void dfSprites::update(void)
{
	int i = 0;
	for (auto object : m_objects) {

		if (object) {
			// if the animation got updated, update the sprite buffers
			if (object->updateSprite(
				&m_positions[i],
				&m_textureIndex[i],
				&m_directions[i]
			)) {
				m_updated = true;
			}
		}
		else {
			// mark the model as non-existent
			m_textureIndex[i] = glm::vec4(65536, 0, 0, 0);
		}
		i++;
	}

	if (m_updated) {
		m_geometry->verticesToDisplay(m_toDisplay);
		m_geometry->dirty();
		m_updated = false;
	}

	if (m_dirtyModels) {
		models->bind();
		models->map(&m_models.models, 0, sizeof(struct GLmodel));
		models->unbind();
		m_dirtyModels = false;
	}
}

/**
 * Remove a sprite from the list
 */
void dfSprites::remove(dfSprite* object)
{
	for (size_t i = 0; i < m_objects.size(); i++) {
		if (m_objects[i] == object) {
			m_objects[i] = nullptr;
			m_updated = true;

			m_freeList.push(i);	// register the empty slot
			break;
		}
	}
}

/**
 *
 */
void dfSprites::OnWorldInsert(void)
{
	fwScene* scene = (fwScene * )g_gaWorld.get("scene");
	scene->addChild(this);
}

/**
 * trigger when from the gaWorld
 */
void dfSprites::OnWorldRemove(void)
{
	fwScene* scene = (fwScene*)g_gaWorld.get("scene");
	scene->removeChild(this);
}

dfSprites::~dfSprites()
{
}
