#include "dfSprite.h"

#include "../../gaEngine/gaWorld.h"

#include "../dfSprites.h"
#include "../dfLevel.h"
#include "../dfModel.h"
#include "../dfModel/dfFME.h"

/**
 * create from an image
 */
dfSprite::dfSprite(dfFME* fme, const glm::vec3& position, float ambient):
	dfObject(fme, position, ambient, OBJECT_FME)
{
	addComponent(&m_componentLogic);
}

/**
 * create from a model
 */
dfSprite::dfSprite(dfModel* model, const glm::vec3& position, float ambient, int type):
	dfObject(model, position, ambient, type)
{
	addComponent(&m_componentLogic);
}

/**
 * create from a model name
 */
dfSprite::dfSprite(const std::string& name, const glm::vec3& position, float ambient, int type) :
	dfObject((dfFME*)g_gaWorld.getModel(name), position, ambient, type)
{
	addComponent(&m_componentLogic);
}

/**
 * Update the sprite buffers if the object is different
 */
bool dfSprite::updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	int updates = 0;

	if (m_dirtyAnimation) {
		texture->r = (float)m_source->id();
		texture->a = m_ambient;
		m_dirtyAnimation = false;
		updates++;
	}

	if (m_dirtyPosition) {
		*position = m_position;
		m_dirtyPosition = false;
		updates++;
	}

	return updates > 0;
}

/**
 * trigger when inserted in a gaWorld
 *  add to the sprite manaer
 */
void dfSprite::OnWorldInsert(void)
{
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->add(this);

	dfObject::OnWorldInsert();
}

/**
 * trigger when from the gaWorld
 *  remove from the sprite manager
 */
void dfSprite::OnWorldRemove(void)
{
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->remove(this);

	dfObject::OnWorldRemove();
}

dfSprite::~dfSprite()
{
}
