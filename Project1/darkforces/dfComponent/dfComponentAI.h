#pragma once

#include <glm/vec3.hpp>

#include "../../gaEngine/gaComponent.h"

class dfComponentAI : public gaComponent
{
	glm::vec3 m_direction = glm::vec3(1.0, 0, 1.0);
	glm::vec3 m_movement = glm::vec3(1.0, 0, 1.0);
	glm::vec3 m_center = glm::vec3(0);
	float alpha=0;
	float m_progress = 0.045f;

public:
	dfComponentAI();

	void dispatchMessage(gaMessage* message) override;				// let a component deal with a situation

	~dfComponentAI();
};