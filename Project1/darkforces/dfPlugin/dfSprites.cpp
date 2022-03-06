#include "dfSprites.h"

#include "../../framework/fwMaterialBasic.h"
#include "../../framework/fwScene.h"

#include "../../gaEngine/gaBoundingBoxes.h"
#include "../../gaEngine/World.h"

static fwMaterialBasic* spriteMaterial = nullptr;
static glUniformBuffer* models = nullptr;
static fwUniform* modelsUniform = nullptr;

DarkForces::Plugins::Sprites g_dfSpritesEngine;

DarkForces::Plugins::Sprites::Sprites(void):
	fwSprites(),
	GameEngine::Plugin("Sprites")
{
	set_name("dfSprites");
}

DarkForces::Plugins::Sprites::Sprites(int nbSprites, dfAtlasTexture* atlas):
	fwSprites(nbSprites),
	GameEngine::Plugin("Sprites")
{
	set_name("dfSprites");

	init(nbSprites, atlas);
}

/**
 * initialise all data for the sprites manager
 */
void DarkForces::Plugins::Sprites::init(int nbSprites, dfAtlasTexture* atlas)
{
	m_size = nbSprites;

	m_objects.resize(nbSprites);
	m_newSprite.resize(nbSprites);
	m_positions.resize(nbSprites);
	m_directions.resize(nbSprites);
	m_textureIndex.resize(nbSprites);

	m_nbObjects = nbSprites;
	m_toDisplay = -1;

	// create a list of available sprites
	for (int i = nbSprites - 1; i >= 0; i--) {
		m_freeList.push(i);
	}

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
void DarkForces::Plugins::Sprites::addModel(dfModel* model)
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
void DarkForces::Plugins::Sprites::add(DarkForces::Component::Sprite* sprite)
{
	// find an empty slot
	size_t slot;

	if (m_freeList.size() == 0) {
		slot = m_objects.size();

		m_objects.resize(slot + 16);
		m_newSprite.resize(slot + 16);
		m_positions.resize(m_positions.size() + 16);
		m_directions.resize(m_positions.size() + 16);
		m_textureIndex.resize(m_positions.size() + 16);

		m_geometry->resizeAttribute("aPos", &m_positions[0], slot + 1);
		m_geometry->resizeAttribute("aDirection", &m_directions[0], slot + 1);
		m_geometry->resizeAttribute("aData", &m_textureIndex[0], slot + 1);

		m_nbObjects++;
	}
	else {
		slot = m_freeList.front();
		m_freeList.pop();
	}

	if (slot >= m_toDisplay) {
		m_toDisplay = slot + 1;
	}

	m_objects[slot] = sprite;
	m_newSprite[slot] = true;
	float g = (float)m_modelsIndex[sprite->model()];
	m_textureIndex[slot].g = g;

	sprite->slot(slot);

	sprite->update(
		&m_positions[slot],
		&m_textureIndex[slot],
		&m_directions[slot]
	);

	m_updated = true;
}

/**
 * Push updated attributes to the GPU
 */
void DarkForces::Plugins::Sprites::update(void)
{
	int i = 0;
	bool b;
	for (auto obj : m_objects) {

		if (obj) {
			DarkForces::Component::Sprite* sprite = static_cast<DarkForces::Component::Sprite*>(obj);
			b = sprite->update(
				&m_positions[i],
				&m_textureIndex[i],
				&m_directions[i]);

			// if the animation got updated, update the sprite buffers
			if (b) {
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
		m_geometry->dirty();
		m_geometry->verticesToDisplay(m_toDisplay);
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
void DarkForces::Plugins::Sprites::remove(DarkForces::Component::Sprite* sprite)
{
	for (size_t i = 0; i < m_objects.size(); i++) {
		if (m_objects[i] == sprite) {
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
void DarkForces::Plugins::Sprites::OnWorldInsert(void)
{
	fwScene* scene = (fwScene * )g_gaWorld.get("scene");
	scene->addChild(this);
}

/**
 * trigger when from the gaWorld
 */
void DarkForces::Plugins::Sprites::OnWorldRemove(void)
{
	fwScene* scene = (fwScene*)g_gaWorld.get("scene");
	scene->removeChild(this);
}

/**
 * // execute the plugin after processing the message queue
 */
void DarkForces::Plugins::Sprites::afterProcessing(void)
{
	update();
}
