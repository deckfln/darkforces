#pragma once

#include <glm/vec3.hpp>

#include "../../framework/fwTransforms.h"
#include "../../gaEngine/gaComponent.h"

class dfComponentAI : public gaComponent
{
	glm::vec3 m_direction = glm::vec3(1.0, 0, 1.0);
	glm::vec3 m_movement = glm::vec3(1.0, 0, 1.0);
	glm::vec3 m_center = glm::vec3(0);
	float m_alpha=0;								// rotation angle for the direction
	float m_progress = 0.045f;						// 
	int m_time = 0;									// time left to move in the direction
	bool m_active = true;							// is AI active
	Framework::fwTransforms m_transforms;			// transforms to move the object

	void tryToMove(void);

public:
	dfComponentAI();

	void dispatchMessage(gaMessage* message) override;				// let a component deal with a situation

	~dfComponentAI();
};