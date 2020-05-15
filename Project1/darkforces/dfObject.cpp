#include "dfObject.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"

dfObject::dfObject(dfModel *source, float x, float y, float z):
	m_source(source),
	m_x(x),
	m_y(y),
	m_z(z)
{
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
}

/**
 * Add the object to a sprites list
 */
void dfObject::addToSprites(dfSprites* sprites)
{
	glm::vec3 level(m_x, m_y, m_z);
	glm::vec3 gl;
	dfLevel::level2gl(level, gl);

	sprites->add(gl, m_source->name(), m_source->textureID());
}

dfObject::~dfObject()
{
}
