#pragma once

#include <string>
#include <vector>

#include "dfLogicStop.h"
#include "../framework/fwMaterial.h"
#include "../framework/fwMesh.h"

class dfLevel;
class dfMesh;

enum {
	DF_ELEVATOR_HOLD,		// elevator is not animated
	DF_ELEVATOR_MOVE,		// is moving
	DF_ELEVATOR_WAIT		// is waiting at a stop
};

class dfLogicElevator {
	std::string m_class;
	int m_speed = 0;					// time in millisecond between 2 stops
	int m_eventMask = 0;
	std::vector<dfLogicStop*> m_stops;	// all stops of the evelator

	std::string m_sector;				// sector that is an evelator
	dfSector* m_pSector = nullptr;

	int m_status = DF_ELEVATOR_HOLD;	// status of the elevator
	time_t m_tick = 0;					// current timer
	unsigned int m_currentStop = 0;		// current stop for the running animation
	unsigned int m_nextStop = 0;		// target altitude

	float m_current = 0;				// current altitude of the part to move (floor or ceiling)
	float m_direction = 0;				// direction and speed of the move
	float m_target = 0;					// target altitude

	dfLevel* m_parent = nullptr;		// level the elevator is on

	dfMesh* m_mesh = nullptr;			// mesh of the elevator

	void moveTo(dfLogicStop* stop);	// extract the floor position from the stop and the 
	void moveTo(float z);			// extract the floor position from the stop and the 
	void move2nextFloor(void);

public:
	dfLogicElevator(std::string& kind, dfSector* sector, dfLevel *parent);
	dfLogicElevator(std::string& kind, std::string& name);
	void speed(int speed) { m_speed = speed * 1000; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	std::string& sector(void) { return m_sector; };
	void sector(dfSector* pSector);
	void parent(dfLevel* parent) { m_parent = parent; };
	void addStop(dfLogicStop* stop);
	fwMesh *buildGeometry(fwMaterial* material);
	void init(int stopID);
	void trigger(std::string& sclass);
	bool animate(time_t delta);
	~dfLogicElevator(void);
};
