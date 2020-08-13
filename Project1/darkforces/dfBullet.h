#pragma once

#include <glm/vec3.hpp>

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"

class fwMesh;
class fwScene;
class gaMessage;

class dfBullet : public gaEntity
{
	glm::vec3 m_direction;
	GameEngine::ComponentMesh m_componentMesh;

public:
	dfBullet(const glm::vec3& position, const glm::vec3& direction);

	void dispatchMessage(gaMessage* message) override;

	~dfBullet();
};