#pragma once

#include "../../dfSpriteAnimated.h"
#include "../../../../framework/math/fwCylinder.h"

#include "../../../../gaEngine/gaComponent/gaAIPerception.h"
#include "../../../../gaEngine/gaComponent/gaCActor.h"
#include "../../../../gaEngine/gaComponent/gaSound.h"

#include "../../../dfComponent/dfWeapon.h"
#include "../../../dfComponent/dfComponentActor.h"
#include "../../../dfComponent/dfMoveEnemy.h"

class dfLevel;

namespace DarkForces {
	class Enemy : public dfSpriteAnimated
	{
		fwCylinder m_cylinder;						// player bounding cylinder
		DarkForces::Component::Actor m_actor;
		DarkForces::Component::MoveEnemy m_ai;
		DarkForces::Component::Weapon m_weapon;
		GameEngine::Component::AIPerception m_aiPerception;
		GameEngine::Component::Sound m_sound;

	public:
		enum Sound {
			DIE,
			FIRE
		};
		Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID);
		void setLevel(dfLevel* level);
		~Enemy();
	};
}