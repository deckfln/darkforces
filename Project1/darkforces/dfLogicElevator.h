#pragma once

#include <string>
#include <vector>

#include "dfLogicStop.h"

class dfLogicElevator {
	std::string m_class;
	int m_speed = 0;
	int m_eventMask = 0;
	std::vector<dfLogicStop*> m_stops;	// token based positions

	std::string m_sector;	// sector that is an evelator
	dfSector* m_pSector = nullptr;

public:
	dfLogicElevator(std::string& kind, std::string& sector);
	void speed(int speed) { m_speed = speed; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	void addStop(dfLogicStop* stop);
	std::string& sector(void) { return m_sector; };
	void sector(dfSector* pSector) { m_pSector = pSector; };
	void init(void);
	void trigger(std::string& sclass) {};
	~dfLogicElevator(void);
};
