#pragma once

#include "../framework/fwCollision.h"
#include "dfLevel.h"

class dfCollision : public fwCollision
{
	dfLevel* m_level = nullptr;

public:
	dfCollision();

	void bind(dfLevel* level) { m_level = level; };
	float ground(glm::vec3& position);
	bool checkEnvironement(glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection);
	~dfCollision(void);
};