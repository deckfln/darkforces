#pragma once

#include <string>
#include <vector>

#include "../config.h"

#include "../framework/fwMaterial.h"
#include "../framework/fwMesh.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaMessage.h"

#include "dfComponent/dfComponentActor.h"

#include "flightRecorder/frElevator.h"

class fwCylinder;
class gaCollisionPoint;
class dfLevel;
class dfMesh;
class dfLogicTrigger;
class gaMessage;
class dfSign;
class dfBitmap;
class dfVOC;
class dfLogicStop;
class dfSector;

class dfElevator: public gaEntity {
public:
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
	enum class Status {
		HOLD,		// elevator is not animated
		MOVE,		// is moving
		WAIT,		// is waiting at a stop
		TERMINATED	// the elevator cannot be activated anymore
	};

	/* Sounds to play when elevator move */
	enum Sound {
		START = 0,	// leaving a stop
		MOVE = 1,	// moving between stops
		END = 2		// arriving at stop
	};

	/* Message send/received */
	enum Message {
		CROSSLINE_FRONT = 1,	// Cross line from front side 
		CROSSLINE_BACK = 2,		// Cross line from back side 
		ENTER_SECTOR = 4,		// Enter sector
		LEAVE_SECTOR = 8,		// Leave sector 
		NUDGE_FRONT_INSIDE = 16,// Nudge line from front side / Nudge sector from inside 
		NUDGE_BACK_OUTSIE = 32,	// Nudge line from back side / Nudge sector from outside 
		EXPLOSION = 64,			// Explosion 
		SHOOT = 128,			//Shoot or punch line(see entity_mask) 
		LAND = 512				// Land on floor of sector
	};

private:
	// static state
	Type m_type = Type::INV;	// class of elevator

	//TODO adapt the default speed
	float m_speed = 20;					// time in millisecond between 2 stops
	int m_eventMask = 0;
	float m_zmin = 99999;				// for elevator inv, standard : extend of the elevator
	float m_zmax = -99999;
	glm::vec3 m_center = glm::vec3(0);	// rotation axis for SPIN1 elevators
	glm::vec3 m_move = glm::vec3(0);	// based off 'angle' for MOVE1 elevators

	float m_p = 0;
	bool m_master = true;				// is the elevator operational ?
	uint32_t m_keys = DarkForces::Keys::NONE;// key activating the elevator

	std::vector<dfLogicStop*> m_stops;	// all stops of the elevator

	std::string m_sector;				// sector that is an elevator
	dfSector* m_pSector = nullptr;

	// Dynamic state
	Status m_status = Status::HOLD;		// status of the elevator
	float m_tick = 0;					// current timer
	float m_delay = 0;					// time to run the elevator
	unsigned int m_currentStop = 0;		// current stop for the running animation
	unsigned int m_nextStop = 0;		// target altitude

	float m_current = 0;				// current altitude of the part to move (floor or ceiling)
	float m_direction = 0;				// direction and speed of the move
	float m_target = 0;					// target altitude

	dfLevel* m_parent = nullptr;		// level the elevator is on
	dfMesh* m_mesh = nullptr;			// mesh of the elevator
	std::list<dfSign*> m_signs;			// list of signs bound to the elevator

	dfVOC* m_sounds[3] = { nullptr, nullptr, nullptr };

	void moveTo(dfLogicStop* stop);
	void moveTo(float z);
	void moveToNextStop(void);
	bool animateMoveZ(void);
	void setSignsStatus(int status);
	void init(const std::string& kind);

public:
	dfElevator(std::string& kind, dfSector* sector, dfLevel *parent);
	dfElevator(std::string& kind, const std::string& name);
	dfElevator(dfElevator *source);

	void speed(float speed) { m_speed = speed; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	std::string& sector(void) { return m_sector; };
	dfSector* psector(void) { return m_pSector; };
	void parent(dfLevel* parent) { m_parent = parent; };
	void center(float x, float y) { m_center.x = x; m_center.y = y; };
	int keys(void) { return m_keys; };
	bool needsKeys(void) { return m_keys != 0; };
	bool is(dfElevator::Type type) { return m_type == type; };
	dfMesh* mesh(void) { return m_mesh; };
	void angle(float angle);
	dfLogicStop* stop(int i);
	void keys(std::string& key);
	void bindSector(dfSector* pSector);
	void addStop(dfLogicStop* stop);
	dfMesh *buildGeometry(fwMaterial* material, std::vector<dfBitmap*>& bitmaps);
	void init(int stopID);
	bool animate(time_t delta);
	void addSign(dfSign*);
	void dispatchMessage(gaMessage* message);

	void getMessagesToSectors(std::list<std::string>& sectors);
	void sound(int effect, dfVOC* sound);

	// flight recorder data
	inline int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::Elevator);
	};														// size of one record
	uint32_t recordState(void* record) override;				// return a record of an actor state (for debug)
	void loadState(flightRecorder::Entity* record) override;// reload an actor state from a record
	void debugGUIChildClass(void) override;					// Add dedicated component debug the entity
	~dfElevator(void);
};
