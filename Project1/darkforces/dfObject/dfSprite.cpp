#include "dfSprite.h"

#include "../../gaEngine/gaWorld.h"
#include "../dfSprites.h"
#include "../dfLevel.h"
#include "../dfModel.h"
#include "../dfModel/dfFME.h"

dfSprite::dfSprite(dfFME* fme, const glm::vec3& position, float ambient):
	dfObject(fme, position, ambient, OBJECT_FME)
{
}

dfSprite::dfSprite(dfModel* model, const glm::vec3& position, float ambient, int type):
	dfObject(model, position, ambient, type)
{
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
 */
void dfSprite::OnWorldInsert(void)
{
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->add((dfSprite*)this);

	dfObject::OnWorldInsert();
}

dfSprite::~dfSprite()
{
}
