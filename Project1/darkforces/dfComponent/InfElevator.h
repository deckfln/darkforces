#pragma once

#include <list>
#include "../../gaEngine/gaComponent.h"

#include "../dfElevator.h"
#include "../dfBitmap.h"

class dfLogicStop;
class dfSector;
class dfLevel;
class dfVOC;
class dfMesh;
class fwMaterial;

namespace GameEngine {
	class ComponentMesh;
}

namespace DarkForces {
	namespace Component {
		/**
		 * Generic elevator class: move between stops at the giving speed
		 * sub-classes will deal with what kind of move it is (morph, spin, light, floor ...)
		 */
		class InfElevator : public gaComponent {
		protected:
			enum class Status {
				HOLD,		// elevator is not animated
				MOVE,		// is moving
				WAIT,		// is waiting at a stop
				TERMINATED	// the elevator cannot be activated anymore
			};

			// static state
			dfElevator::Type m_type = dfElevator::Type::INV;	// class of elevator
			uint32_t m_eventMask = 0;
			float m_speed = 20;					// time in millisecond between 2 stops
			std::vector<dfLogicStop*> m_stops;	// all stops of the elevator

			const std::string& m_sector;		// sector that is an elevator
			dfSector* m_pSector = nullptr;

			float m_zmin = INFINITY;			// range of the elevator
			float m_zmax = -INFINITY;
			glm::vec3 m_center = glm::vec3(0);	// base point (rotation point for SPIN , origin for MORH...)

			// Dynamic state
			Status m_status = Status::HOLD;		// status of the elevator
			float m_tick = 0;					// current timer
			float m_delay = 0;					// time to run the elevator
			uint32_t m_currentStop = 0;			// current stop for the running animation
			uint32_t m_nextStop = 0;			// target stop

			float m_current = 0;				// current position
			float m_direction = 0;				// direction and speed of the move
			float m_target = 0;					// target altitude

			dfLevel* m_parent = nullptr;		// level the elevator is on

			dfVOC* m_sounds[3] = { nullptr, nullptr, nullptr };

			virtual void moveTo(float z);		// move the given position (depend on the elevator type)
			void moveTo(dfLogicStop* stop);		// move directly to the given stop
			void moveToNextStop(void);			// start moving to the next stop
			bool animate(time_t delta);			// move between stops

		public:
			/* Sounds to play when elevator move */
			enum Sound {
				START = 0,	// leaving a stop
				MOVE = 1,	// moving between stops
				END = 2		// arriving at stop
			};

			InfElevator(const std::string& sector);
			InfElevator(dfSector* sector);

			// getter/setter
			inline void speed(float speed) { m_speed = speed; };
			void eventMask(uint32_t eventMask);
			inline const std::string& sector(void) { return m_sector; };
			inline dfSector* psector(void) { return m_pSector; };
			inline void center(float x, float y) { m_center.x = x; m_center.y = y; };

			void addStop(dfLogicStop* stop);			// add a stop and update the range of the elevator
			void addSound(uint32_t when, dfVOC* sound) {
				m_sounds[when] = sound;
			}

			void dispatchMessage(gaMessage* message) override;

			virtual dfMesh* buildGeometry(fwMaterial* material, std::vector<dfBitmap*>& bitmaps) { return nullptr;	};
			GameEngine::ComponentMesh* buildComponentMesh(void);

			// flight recorder status
			inline uint32_t recordSize(void);		// size of the component record
			uint32_t recordState(void* record);			// save the component state in a record
			uint32_t loadState(void* record);			// reload a component state from a record

			// debugger
			void debugGUIinline(void) override;			// display the component in the debugger

		};
	}
}