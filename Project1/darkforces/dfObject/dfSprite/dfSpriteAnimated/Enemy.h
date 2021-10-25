#pragma once

#include "../../dfSpriteAnimated.h"
#include "../../../../framework/math/fwCylinder.h"

#include "../../../../gaEngine/gaComponent/gaAIPerception.h"
#include "../../../../gaEngine/gaComponent/gaCActor.h"

#include "../../../dfComponent/dfWeapon.h"

namespace DarkForces {
	class Enemy : public dfSpriteAnimated
	{
		fwCylinder m_cylinder;						// player bounding cylinder
		GameEngine::Component::AIPerception m_aiPerception;
		GameEngine::Component::Actor m_actor;
		DarkForces::Component::Weapon m_weapon;


	public:
		Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID);
		~Enemy();
	};
}