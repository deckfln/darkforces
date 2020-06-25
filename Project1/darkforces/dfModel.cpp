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
 * refresh the boundingbox based on m_size_gl & m_insert_gl
 */
void dfModel::updateBoundingBox(void)
{
	m_bounding_gl.set(
		-m_size_gl.x / 2, m_insert_gl.y, -m_size_gl.x / 2,
		m_size_gl.x / 2, m_size_gl.y + m_insert_gl.y, m_size_gl.x / 2
	);
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
	m_id = id;
}