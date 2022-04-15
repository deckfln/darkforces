#pragma once

#include <vector>

#include "../../gaEngine/gaComponent.h"
#include "../../gaEngine/gaImage2D.h"
#include "../gaItem/dfItem/dfWeapon.h"

class fwTexture;

namespace DarkForces {
	namespace Component {
		class Weapon : public gaComponent {
			DarkForces::Weapon* m_current=nullptr;
			float m_ratio = 1.6f;									// screen current ratio
			float m_lookDownUp = 0;									// looking up or down (in pixels)
			glm::vec3 m_wobbling = glm::vec3(0);					// waving when moving
			uint32_t m_frameStartMove = 0;							// when did the move start
			float m_wobblingT = 0;									// time of wobbling
			float m_wobblingDirection = 1;							//
			bool m_walking = false;									// entity is walking, activate the wobling
			uint32_t m_currentImage = 0;							// display image #i from the weapon images
			uint32_t m_timer = 0;									// number of frames before changing the image
			void setImage(void);									// define image position on screen


		public:
			Weapon(void);
			Weapon(DarkForces::Weapon* current);
			~Weapon(void);
			DarkForces::Weapon* set(DarkForces::Weapon* weapon);	// set the kind of weapon and return filename of HUD
			DarkForces::Weapon* get(void);							// return data on weapons

			inline void setActorPosition(const glm::vec2& v) {
				m_ActorPosition = v;
			};														// force the position of the weapon the player
			void addEnergy(int32_t value);							// add energy to the weapon

			void dispatchMessage(gaMessage* message) override;

			GameEngine::Image2D* getImage(void);					// build and return an Image2D for the HUD

			// debugger
			void debugGUIinline(void) override;

		protected:
			DarkForces::Weapon::Kind m_kind;
			glm::vec2 m_ActorPosition=glm::vec2(0);				// position of the weapon on the actor

			time_t m_time=0;									// world time of the last fire (when the player keep the fire button down)

			GameEngine::Image2D* m_image = nullptr;				// 2D image of the weapon
			glm::vec4 m_material = glm::vec4(0);				// Weapon material for the shader

			void onChangeWeapon(gaMessage* message);			// change the current weapon
			void onFire(const glm::vec3& direction, time_t time);	// single shot
			void onStopFire(gaMessage* message);				// keep the finger on the trigger
			void onChangeLightning(gaMessage* message);			// when the player enter a new sector light
			void onScreenResize(gaMessage* message);			// when the screen gets resized
			void onLookAt(gaMessage* message);					// when the player looks somewhere, move the position of the weapon
			void onMove(gaMessage* message);					// when the player walks, move the weapon
			void onWalkStart(gaMessage* message);				// start the wobling
			void onWalkStop(gaMessage* message);				// reset the wobling
			void onTimer(gaMessage* message);					// change the weapon image after XX frames
		};
	}
}