#pragma once

#include "../framework/fwCollision.h"
#include "dfLevel.h"

class dfCollision : public fwCollision
{
	dfLevel* m_level = nullptr;

public:
	dfCollision(void);

	void bind(dfLevel* level) { m_level = level; };
	float ground(glm::vec3& position);
	bool wall(glm::vec3& position, glm::vec3& direction, glm::vec3& intersection);
	~dfCollision(void);
};