#include "gaEntity.h"

gaEntity::gaEntity(const std::string& name) :
	m_name(name)
{
}

gaEntity::gaEntity(const std::string& name, const glm::vec3& position):
	m_name(name),
	m_position(position)
{
}

gaEntity::~gaEntity()
{
}
