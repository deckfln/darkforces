#pragma once

#include <glm/vec3.hpp>
#include "../../gaEngine/gaComponent/gaCActor.h"

#include "../dfSector.h"
#include "../dfLevel.h"
#include "../dfConfig.h"

#include "../../config.h"

namespace DarkForces {

	struct ActorClass {
		uint32_t life;
		uint32_t shield;
		glm::vec2 weapon;
	};

	namespace Component {
		class Actor : public GameEngine::Component::Actor
		{
			std::string m_class;	// class of the actor: player, officer, commando ...
			int32_t m_shield = 100;
			int32_t m_maxShield = 300;

			int32_t m_battery = 0;
			int32_t m_life = 100;

			DarkForces::Keys m_keys = DarkForces::Keys::RED;

			dfSector* m_currentSector = nullptr;	// in what sector is the sector located
			dfLevel* m_level = nullptr;				// fast access to the loaded level

			// module to play a death animation (falling back)
			float m_dyingDelta;						// percentage of the falling back for each frame
			bool m_dying = false;					// if the dying animation playing ?
			glm::vec3 m_dyingDirection;				// direction the actor is falling back when dying

			void setDataFromClass(void);			// extract data from the class
			void onHitBullet(int32_t value);		// hit by a bullet, reduce shield and life
			void onDying(gaMessage* message);		// when the dying animation starts
			void onAnimStart(gaMessage* message);	// when animations starts
			void onAnimNextFrame(gaMessage* message);	// when animations are running
			void onDead(gaMessage* message);		// when the dying animation stops

		public:
			enum Sound {
				DIE,
				FIRE,
				HURT
			};

			Actor(const std::string& xclass);
			Actor(void);
			void addShield(int32_t value);
			void die(void);											// kill the actor

			inline dfSector* currentSector(void) { return m_currentSector; };
			inline void currentSector(dfSector* sector) { m_currentSector = sector; };
			inline void bind(dfLevel* level) { m_level = level; };
			inline DarkForces::Keys keys(void) { return m_keys; };
			inline bool dying(void) { return m_dying; };
			inline void setClass(const std::string& xclass) { 
				m_class = xclass;
				setDataFromClass();
			};
			const struct ActorClass* getActorClass(void);			// return data on the class the actor is from (commando, officer ...)

			void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation

			// flight recorder status
			inline uint32_t recordSize(void) override;			// size of the component record
			uint32_t recordState(void* record) override;		// save the component state in a record
			uint32_t loadState(void* record)override;			// reload a component state from a record

			//debugger
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}