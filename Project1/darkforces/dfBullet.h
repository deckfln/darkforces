#pragma once

#include <glm/vec3.hpp>

#include "../../framework/fwTransforms.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"

class fwMesh;
class fwScene;
class gaMessage;

class dfBullet : public gaEntity
{
	Framework::fwTransforms m_transforms;			// transforms to move the object
	glm::vec3 m_direction;
	GameEngine::ComponentMesh m_componentMesh;

	void tryToMove(void);

public:
	dfBullet(const glm::vec3& position, const glm::vec3& direction);

	void dispatchMessage(gaMessage* message) override;

	~dfBullet();
};