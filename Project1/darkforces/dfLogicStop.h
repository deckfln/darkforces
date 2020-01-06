#pragma once
#include <list>

#include "../framework/fwAABBox.h"
class dfSector;

class dfLogicStop {
	int m_flag = 0;			// content of the stop
		// 1 : absolute
		// 2 : relative
		// 4 : based on antoher sector

		// 8 : time to spend at a stop
		// 16 : action at the stop

	int m_absolute = 0;		// absolute stop of the elevator
	int m_relatiave = 0;		// OR relative stop from the last position
	std::string m_sector;	// 

	int m_time = 0;	// time to stop the elevator a position (absolute or relative or sector)

	// [hold elevator] will remain at stop indefinitely 
	// [terminate] elevator will stay at the stop permanently 
	// [complete] mission will be complete when elev arrives at stop
	std::string m_action;
public:
	void absolute(int absolute) { m_flag |= 1; m_absolute = absolute; };
	void relative(int relative) { m_flag |= 2; m_relatiave = relative; };
	void sector(std::string& sector) { m_flag |= 4; m_sector = sector; };
	void time(int time) { m_flag |= 8; m_time = time; };
	void action(std::string& action) { m_flag |= 16; m_action = action; };
};

class dfLogicElevator {
	std::string m_class;
	int m_speed;
	int m_eventMask;
	std::list<dfLogicStop*> m_stops;

	std::string m_sector;	// sector that is an evelator
	dfSector* m_pSector = nullptr;

public:
	dfLogicElevator(std::string& kind, std::string& sector) { m_class = kind; m_sector = sector; };
	void speed(int speed) { m_speed = speed; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	void addStop(dfLogicStop* stop) { m_stops.push_back(stop); };
	std::string& sector(void) { return m_sector; };
	void sector(dfSector* pSector) { m_pSector = pSector; };
	void trigger(std::string& sclass) {};
	~dfLogicElevator(void) {
		for (auto stop : m_stops) {
			delete stop;
		}
	};
};

