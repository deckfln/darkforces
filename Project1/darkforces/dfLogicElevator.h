#pragma once

#include <string>
#include <vector>

#include "dfMessageClient.h"
#include "dfLogicStop.h"

#include "../framework/fwMaterial.h"
#include "../framework/fwMesh.h"
#include "dfMessage.h"

class dfLevel;
class dfMesh;
class dfLogicTrigger;
class dfMessage;
class dfSign;

enum {
	DF_ELEVATOR_HOLD,		// elevator is not animated
	DF_ELEVATOR_MOVE,		// is moving
	DF_ELEVATOR_WAIT		// is waiting at a stop
};

enum {
	DF_ELEVATOR_INV,		// moving up
	DF_ELEVATOR_BASIC,		// moving down
	DF_ELEVATOR_MOVE_FLOOR,	
	DF_ELEVATOR_CHANGE_LIGHT,
	DF_ELEVATOR_MOVE_CEILING,
	DF_ELEVATOR_MORPH_SPIN1
};

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

class dfLogicElevator: public dfMessageClient {
	std::string m_class;
	int m_type = -1;					// class of elevator
	//TODO adapt the default speed
	float m_speed = 20;					// time in millisecond between 2 stops
	int m_eventMask = 0;
	glm::vec3 m_center = glm::vec3(0);
	float m_p = 0;
	bool m_master = true;				// is the elevator operational ?

	std::vector<dfLogicStop*> m_stops;	// all stops of the evelator

	std::string m_sector;				// sector that is an evelator
	dfSector* m_pSector = nullptr;

	int m_status = DF_ELEVATOR_HOLD;	// status of the elevator
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

	dfMessage m_msg_animate = dfMessage(DF_MESSAGE_TIMER);

	void moveTo(dfLogicStop* stop);
	void moveTo(float z);
	void moveToNextStop(void);
	bool animateMoveZ(void);
	void setSignsStatus(int status);

public:
	dfLogicElevator(std::string& kind, dfSector* sector, dfLevel *parent);
	dfLogicElevator(std::string& kind, std::string& name);

	void speed(float speed) { m_speed = speed; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	std::string& sector(void) { return m_sector; };
	dfSector* psector(void) { return m_pSector; };
	void parent(dfLevel* parent) { m_parent = parent; };
	void center(float x, float y) { m_center.x = x; m_center.y = y; };
	dfLogicStop* stop(int i);

	void bindSector(dfSector* pSector);
	void addStop(dfLogicStop* stop);
	fwMesh *buildGeometry(fwMaterial* material);
	void init(int stopID);
	bool animate(time_t delta);
	bool is(int type) { return m_type == type; };
	void addSign(dfSign*);
	void dispatchMessage(dfMessage* message);
	bool checkCollision(glm::vec3& position, float radius, glm::vec3& intersection);
	~dfLogicElevator(void);
};
