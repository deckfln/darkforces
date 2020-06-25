#include "dfSprite.h"

#include "../dfLevel.h"
#include "../dfModel.h"
#include "../dfModel/dfFME.h"

dfSprite::dfSprite(dfFME* fme, glm::vec3& position, float ambient):
	dfObject(fme, position, ambient, OBJECT_FME)
{
}

dfSprite::dfSprite(dfModel* model, glm::vec3& position, float ambient, int type):
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
		*position = m_position_gl;
		m_dirtyPosition = false;
		updates++;
	}

	return updates > 0;
}

dfSprite::~dfSprite()
{
}
