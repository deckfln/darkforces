#pragma once

#include <glm/vec3.hpp>

#include "../../framework/math/Segment.h"
#include "../../framework/fwTransforms.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"

class fwMesh;
class fwScene;
class gaMessage;

class dfBullet : public gaEntity
{
	glm::vec3 m_direction;
	GameEngine::ComponentMesh m_componentMesh;
	Framework::Segment m_segment;

	void tryToMove(void);

public:
	dfBullet(const glm::vec3& position, const glm::vec3& direction);

	void dispatchMessage(gaMessage* message) override;

	~dfBullet();
};