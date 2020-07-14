#pragma once

#include <string>
#include <vector>

#include "dfLogicStop.h"

#include "../config.h"

#include "../framework/fwMaterial.h"
#include "../framework/fwMesh.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaMessage.h"

#include "dfComponent/dfComponentActor.h"
#include "dfLogicElevatorConst.h"

class fwCylinder;
class gaCollisionPoint;
class dfLevel;
class dfMesh;
class dfLogicTrigger;
class gaMessage;
class dfSign;
class dfBitmap;
class dfVOC;

enum {
	DF_ELEVATOR_CROSSLINE_FRONT = 1,	// Cross line from front side 
	DF_ELEVATOR_CROSSLINE_BACK = 2,		// Cross line from back side 
	DF_ELEVATOR_ENTER_SECTOR = 4,		// Enter sector
	DF_ELEVATOR_LEAVE_SECTOR = 8,		// Leave sector 
	DF_ELEVATOR_NUDGE_FRONT_INSIDE = 16,// Nudge line from front side / Nudge sector from inside 
	DF_ELEVATOR_NUDGE_BACK_OUTSIE = 32,	// Nudge line from back side / Nudge sector from outside 
	DF_ELEVATOR_EXPLOSION = 64,			// Explosion 
	DF_ELEVATOR_SHOOT = 128,			//Shoot or punch line(see entity_mask) 
	DF_ELEVATOR_LAND = 512				// Land on floor of sector
} ;

/* Sounds to play when elevator move
 */
enum dfElevatorSound {
	START = 0,	// leaving a stop
	MOVE = 1,	// moving between stops
	END = 2		// arriving at stop
};

class dfLogicElevator: public gaEntity {
	dfElevatorType m_type = dfElevatorType::INV;	// class of elevator

	//TODO adapt the default speed
	float m_speed = 20;					// time in millisecond between 2 stops
	int m_eventMask = 0;
	float m_zmin = 99999;					// for elevator inv, standard : extend of the elevator
	float m_zmax = -99999;
	glm::vec3 m_center = glm::vec3(0);	// rotation axis for SPIN1 elevators
	glm::vec3 m_move = glm::vec3(0);	// based off 'angle' for MOVE1 elevators

	float m_p = 0;
	bool m_master = true;				// is the elevator operational ?
	int m_keys = DF_KEY_NONE;			// key activating the elevator

	std::vector<dfLogicStop*> m_stops;	// all stops of the evelator

	std::string m_sector;				// sector that is an evelator
	dfSector* m_pSector = nullptr;

	dfElevatorStatus m_status = dfElevatorStatus::HOLD;	// status of the elevator
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

	gaMessage m_msg_animate = gaMessage(GA_MSG_TIMER);

	void moveTo(dfLogicStop* stop);
	void moveTo(float z);
	void moveToNextStop(void);
	bool animateMoveZ(void);
	void setSignsStatus(int status);
	void init(const std::string& kind);

public:
	dfLogicElevator(std::string& kind, dfSector* sector, dfLevel *parent);
	dfLogicElevator(std::string& kind, std::string& name);

	void speed(float speed) { m_speed = speed; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	std::string& sector(void) { return m_sector; };
	dfSector* psector(void) { return m_pSector; };
	void parent(dfLevel* parent) { m_parent = parent; };
	void center(float x, float y) { m_center.x = x; m_center.y = y; };
	int keys(void) { return m_keys; };
	bool needsKeys(void) { return m_keys != 0; };
	bool is(dfElevatorType type) { return m_type == type; };
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
	bool RcheckCollision(float step, glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection);
	bool checkCollision(fwAABBox& box);

	void getMessagesToSectors(std::list<std::string>& sectors);
	void sound(int effect, dfVOC* sound);

	virtual bool checkCollision(fwCylinder& bounding,
		glm::vec3& direction,
		glm::vec3& intersection,
		std::list<gaCollisionPoint>& collisions);		 // extended collision test after a sucessfull AABB collision

	~dfLogicElevator(void);
};
