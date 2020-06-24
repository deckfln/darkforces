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
 * return a mesh for the AABB
 */
fwMesh* dfSprite::drawBoundingBox(void)
{
	m_meshAABB = m_source->drawBoundingBox()->clone();
	m_meshAABB->translate(m_position_gl);

	return m_meshAABB;
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
		dfLevel::level2gl(m_position_lvl, m_position_gl);

		// take the opportunity to update the world bounding box
		m_worldBounding.translateFrom(m_source->bounding(), m_position_gl);

		if (m_meshAABB) {
			// and update the gl boundingbox
			m_meshAABB->translate(m_position_gl);
		}

		*position = m_position_gl;
		m_dirtyPosition = false;
		updates++;
	}

	return updates > 0;
}

dfSprite::~dfSprite()
{
}
