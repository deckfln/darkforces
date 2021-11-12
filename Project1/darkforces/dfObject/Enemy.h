#pragma once

#include "../../framework/math/fwCylinder.h"

#include "../../gaEngine/gaComponent/gaAIPerception.h"
#include "../../gaEngine/gaComponent/gaCActor.h"
#include "../../gaEngine/gaComponent/gaSound.h"

#include "../dfObject.h"
#include "../dfComponent/dfCWeapon.h"
#include "../dfComponent/dfComponentActor.h"
#include "../dfComponent/dfEnemyAI.h"

class dfLevel;
class dfWAX;

namespace DarkForces {
	class Enemy : public DarkForces::Object
	{
		fwCylinder m_cylinder;						// player bounding cylinder
		DarkForces::Component::Actor m_actor;
		DarkForces::Component::EnemyAI m_ai;
		DarkForces::Component::Weapon m_weapon;
		GameEngine::Component::AIPerception m_aiPerception;
		GameEngine::Component::Sound m_sound;

	public:
		Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID);
		void setLevel(dfLevel* level);
		~Enemy();
	};
}