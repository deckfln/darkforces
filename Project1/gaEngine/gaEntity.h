#pragma once

#include <string>
#include <glm/vec3.hpp>
#include "../framework/fwAABBox.h"

class gaEntity
{
protected:
	std::string m_name;
	glm::vec3 m_position = glm::vec3(0);	// position in gl space
	fwAABBox m_worldBounding;				// bounding box in world gl space
public:
	gaEntity(const std::string& name);
	gaEntity(const std::string& name, const glm::vec3& position);
	virtual void collideWith(gaEntity*) {};
	~gaEntity();
};