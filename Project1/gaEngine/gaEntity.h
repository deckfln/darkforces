#pragma once

#include <string>
#include <glm/vec3.hpp>
#include "../framework/fwAABBox.h"

class gaEntity
{
protected:
	std::string m_name;
	int m_entityID = 0;
	int m_class = 0;
	glm::vec3 m_position = glm::vec3(0);	// position in gl space
	fwAABBox m_worldBounding;				// bounding box in world gl space
public:
	gaEntity(int mclass, const std::string& name);
	gaEntity(int mclass, const std::string& name, const glm::vec3& position);
	bool is(int mclass) { return m_class == m_class; };
	virtual void collideWith(gaEntity*) {};
	~gaEntity();
};