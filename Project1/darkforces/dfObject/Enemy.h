#pragma once

#include "../../framework/math/fwCylinder.h"

#include "../../gaEngine/gaComponent/gaAIPerception.h"
#include "../../gaEngine/gaComponent/gaCActor.h"
#include "../../gaEngine/gaComponent/gaSound.h"
#include "../../gaEngine/gaComponent/gaCInventory.h"

#include "../dfObject.h"
#include "../dfComponent/dfCWeapon.h"
#include "../dfComponent/dfCActor.h"
#include "../dfComponent/dfEnemyAI.h"
#include "../dfWeapon.h"
#include "../gaItem/dfItem/dfEnergyClip.h"

class dfLevel;
class dfWAX;

namespace DarkForces {
	class Enemy : public DarkForces::Object
	{
		fwCylinder m_cylinder;								// player bounding cylinder
		DarkForces::Component::Actor m_actor;
		DarkForces::Component::EnemyAI m_ai;
		DarkForces::Component::Weapon m_weapon;
		GameEngine::Component::AIPerception m_aiPerception;
		GameEngine::Component::Sound m_sound;
		GameEngine::Component::Inventory m_inventory;

		DarkForces::EnergyClip m_clip;						// shared clip to store energy
		DarkForces::Weapon m_currentWeapon;					// default weapon the enemy holds

		void onDie(gaMessage* message);						// drop items when dying

	public:
		Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID);
		void extend(void) override;							// extend the object at the end of the load

		void setLevel(dfLevel* level);

		void dispatchMessage(gaMessage* message) override;

		~Enemy();
	};
}