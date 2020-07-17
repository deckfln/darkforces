#pragma once

#include <glm/vec3.hpp>

#include "../gaEngine/gaEntity.h"

class fwMesh;
class fwScene;
class gaMessage;

class dfBullet : public gaEntity
{
	glm::vec3 m_direction;
	gaMessage* m_animate_msg = nullptr;

public:
	dfBullet(const glm::vec3& position, const glm::vec3& direction);
	fwMesh* mesh(void) { return m_mesh; };

	void OnWorldInsert(void) override;				// trigger when the bullet is added to the world
	void dispatchMessage(gaMessage* message) override;

	~dfBullet();
};