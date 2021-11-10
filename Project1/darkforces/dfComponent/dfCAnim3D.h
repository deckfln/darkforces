#pragma once

#include "../../gaEngine/gaComponent.h"

class dfVue;
namespace DarkForces {
	namespace Component {
		class Anim3D : public gaComponent
		{
			glm::vec3 m_animRotationAxe = glm::vec3(0);
			float m_aniRotationSpeed = 0;						// rotates from -999 (max anti-clockwise) to 999 (max clockwise)

			dfVue* m_vue = nullptr;
			bool m_pause = false;								// pause at the end of the animation

			time_t m_lastFrame = 0;								// timestamps of the last frame

			void onTimer(gaMessage* message);					// update based on timer
			void onRotationChange(gaMessage* message);

		public:
			Anim3D(void);
			Anim3D(dfVue* vue);
			Anim3D(const std::string& vue, const std::string& component);
			void set(dfVue* vue);
			void set(const std::string& vue, const std::string& component);
			void pause(bool p) { m_pause = p; };

			void dispatchMessage(gaMessage* message) override;

			// flight recorder and debugger
			/*
			inline int recordSize(void) override {
				return sizeof(flightRecorder::DarkForces::Object3D);
			}													// size of one record
			uint32_t recordState(void* record) override;		// return a record of the entity state (for debug)
			void loadState(void* record) override;				// reload an entity state from a record
			*/

			// debugger
			void debugGUIinline(void);							// Add dedicated component debug the entity


			~Anim3D(void);
		};
	}
}
