#pragma once

#include <string>
#include <glm/vec2.hpp>
#include "../framework/fwAABBox.h"
#include "dfLogicStop.h"

class dfSector;

class dfLogicTrigger {
	std::string m_class;
	int m_eventMask = 0;
	std::string m_client;			// name of the target sector 
	dfLogicElevator* m_pClient = nullptr;	// pointer to the sector's elevator

	std::string m_sector;			// sector that host the trigger
	int m_wallIndex = -1;			// index of the wall being a trigger

	fwAABBox m_boundingBox;			// bouding box of the trigger

public:
	dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex);
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	void client(std::string& client) { m_client = client; };
	void evelator(dfLogicElevator* pClient) { m_pClient = pClient; };
	std::string& sector(void) { return m_sector; };
	std::string& client(void) { return m_client; };
	int wall(void) { return m_wallIndex; };
	void boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling);
	bool collide(fwAABBox& box);
	void activate(void);
};