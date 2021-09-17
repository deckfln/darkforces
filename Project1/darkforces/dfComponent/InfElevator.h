#pragma once

#include <list>
#include <vector>

#include "../../gaEngine/gaComponent.h"

#include "../../config.h"
#include "../dfBitmap.h"
#include "../dfSector.h"

class dfLogicStop;
class dfSector;
class dfLevel;
class dfMesh;
class fwMaterial;
class dfVOC;

namespace GameEngine {
	class ComponentMesh;
}

namespace DarkForces {
	namespace Component {

		class Trigger;

		/**
		 * Generic elevator class: move between stops at the giving speed
		 * sub-classes will deal with what kind of move it is (morph, spin, light, floor ...)
		 */
		class InfElevator : public gaComponent {
		public:
			/* Sounds to play when elevator move */
			enum Sound {
				START = 0,	// leaving a stop
				MOVE = 1,	// moving between stops
				END = 2		// arriving at stop
			};
			enum class Type {
				INV,		// moving up
				BASIC,		// moving down
				MOVE_FLOOR,
				CHANGE_LIGHT,
				MOVE_CEILING,
				MORPH_SPIN1,
				MORPH_MOVE1,
				MORPH_SPIN2,
				DOOR
			};
			enum class Status : uint32_t {
				HOLD,		// elevator is not animated
				MOVE,		// is moving
				WAIT,		// is waiting at a stop
				TERMINATED	// the elevator cannot be activated anymore
			};

			InfElevator(const std::string& sector, bool smart = false);
			InfElevator(Type kind, dfSector* sector, bool smart = false);

			// getter/setter
			void eventMask(uint32_t eventMask);
			inline uint32_t eventMask(void) { return m_eventMask; };
			inline const std::string& sector(void) { return m_sector; };
			inline dfSector* psector(void) { return m_pSector; };
			inline void center(float x, float y) { m_center.x = x; m_center.y = y; };
			inline Type type(void) { return m_type; };
			inline void speed(float speed) { m_speed = speed; };
			inline float zmin(void) { return m_zmin; };
			inline float zmax(void) { return m_zmax; };
			void key(const std::string& key);				// register the needed key
			inline DarkForces::Keys key(void) { return m_key; };
			inline Status status(void) { return m_status; };

			inline void addTrigger(Trigger* trigger) { 
				m_triggers.push_back(trigger); 
			};
			inline const std::vector<Trigger*>& getTriggers(void) {
				return m_triggers;
			}

			void prepareMesh(void);							// set the mesh data before the final build
			void meshData(float bottom, float top, uint32_t texture, bool clockwise, dfWallFlag whatToDraw);	// set the mesh data

			void addStop(dfLogicStop* stop);				// add a stop and update the range of the elevator
			void addSound(uint32_t action, dfVOC* sound);	// register a sound for a SART, MOVE, STOP

			void gotoStop(uint32_t stop);					// Force an elevator to go to a specific Stop

			void dispatchMessage(gaMessage* message) override;

			virtual dfMesh* buildMesh(void);				// build the dfMesh of the elevator
			virtual void relocateMesh(dfMesh* mesh) {};		// move the mesh vertices into a 0,0,0 position

			// flight recorder status
			inline uint32_t recordSize(void);				// size of the component record
			uint32_t recordState(void* record);				// save the component state in a record
			uint32_t loadState(void* record);				// reload a component state from a record

			// debugger
			void debugGUIinline(void) override;				// display the component in the debugger

		protected:

			// static state
			Type m_type = Type::INV;						// class of elevator
			uint32_t m_eventMask = 0;
			float m_speed = 20;								// time in millisecond between 2 stops
			bool m_smart = false;							// TODO react to smart objects
			std::vector<dfLogicStop*> m_stops;				// all stops of the elevator
			Keys m_key = Keys::NONE;						// needed key to activate the elevator

			const std::string& m_sector;					// sector that is an elevator
			dfSector* m_pSector = nullptr;

			float m_zmin = INFINITY;						// range of the elevator
			float m_zmax = -INFINITY;
			glm::vec3 m_center = glm::vec3(0);				// base point (rotation point for SPIN , origin for MORH...)

			std::vector<Trigger*> m_triggers;				// list of triggers that can activate that elevator

			// data to build a mesh
			bool m_meshData = false;						// no value was set
			float m_meshBottom = 0;							// lower z value
			float m_meshCeiling = 0;						// upper z value
			uint32_t m_meshTexture = 0;						// texture to use
			bool m_meshClockwise = false;					// draw walls clockwise or not
			dfWallFlag m_meshFlag = dfWallFlag::ALL;		// what walls to draw

			// Dynamic state
			Status m_status = Status::HOLD;					// status of the elevator
			float m_tick = 0;								// current timer
			float m_delay = 0;								// time to run the elevator
			uint32_t m_currentStop = 0;						// current stop for the running animation
			uint32_t m_nextStop = 0;						// target stop

			float m_current = 0;							// current position
			float m_direction = 0;							// direction and speed of the move
			float m_target = 0;								// target altitude

			dfLevel* m_parent = nullptr;					// level the elevator is on

			virtual void moveTo(float z);					// move the given position (depend on the elevator type)
			void moveTo(dfLogicStop* stop);					// move directly to the given stop
			void moveToNextStop(void);						// start moving to the next stop
			bool animate(time_t delta);						// move between stops
		};
	}
}