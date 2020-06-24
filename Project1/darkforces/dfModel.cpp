#include "dfModel.h"

dfModel::dfModel(std::string& name):
	m_name(name)
{
}

bool dfModel::named(std::string& name)
{
	return m_name == name;
}

/**
 * Create a mesh to represent the boundingbox (gl model space)
 */
fwMesh* dfModel::drawBoundingBox(void)
{
	if (m_meshAABB == nullptr) {
		m_meshAABB = m_bounding_gl.draw();
	}
	return m_meshAABB;
}

/**
 * update the spriteModel based on the model
 */
void dfModel::spriteModel(GLmodel& model, int id)
{
	// take the opportunity to update the model gl space bounding box
	SpriteModel* sm = &model.models[id];

	m_bounding_gl.set(
		-sm->size.x / 2, sm->insert.y, -sm->size.x / 2,
		sm->size.x / 2, sm->size.y + sm->insert.y, sm->size.x / 2
	);

	m_id = id;
}

