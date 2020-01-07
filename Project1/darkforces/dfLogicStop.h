#pragma once
#include <string>
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

	float m_absolute = 0;		// absolute stop of the elevator
	float m_relatiave = 0;	// OR relative stop from the last position
	std::string m_sector;	// based on sector XXX
	dfSector *m_pSector;	// based on sector XXX

	float m_time = 0;			// time to stop the elevator a position (absolute or relative or sector)

	// [hold elevator] will remain at stop indefinitely 
	// [terminate] elevator will stay at the stop permanently 
	// [complete] mission will be complete when elev arrives at stop
	std::string m_action;
public:
	void absolute(float absolute) { m_flag |= 1; m_absolute = absolute; };
	void relative(float relative) { m_flag |= 2; m_relatiave = relative; };
	void sector(std::string& sector) { m_flag |= 4; m_sector = sector; };
	std::string& sector(void) { return m_sector; };
	void sector(dfSector* pSector) { m_pSector = pSector; };
	void time(float time) { m_flag |= 8; m_time = time; };
	void action(std::string& action) { m_flag |= 16; m_action = action; };
	std::string action(void) { return m_action; };
	bool isTimeBased(void) { return (m_flag | 8) == 8; };
	float z_position(float);
};