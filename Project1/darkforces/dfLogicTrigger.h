#pragma once

#include <string>
#include <map>
#include <list>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "dfLogicElevator.h"

class dfSector;

class dfLogicTrigger {
	std::string m_class;
	int m_eventMask = 0;
	std::list<std::string> m_clients;		// name of the target sector 
	std::list<dfLogicElevator*> m_pClients;  // pointer to the target sector 

	std::string m_sector;			// sector that host the trigger
	int m_wallIndex = -1;			// index of the wall being a trigger

	fwAABBox m_boundingBox;			// bouding box of the trigger
	glm::vec3 m_boundingBoxCenter;	// original position of the bounding box
	glm::vec3 m_boundingBoxSize;	// original size of the bounding box

public:
	dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex);
	dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex, dfLogicElevator *client);
	dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client);

	void eventMask(int eventMask) { m_eventMask = eventMask; };

	void client(std::string& client) { m_clients.push_back(client); }
	std::list<std::string>& clients(void) { return m_clients; };
	void evelator(dfLogicElevator* pClient) { m_pClients.push_back(pClient); };
	std::string& sector(void) { return m_sector; };
	int wall(void) { return m_wallIndex; };
	void boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling);
	bool collide(fwAABBox& box);
	void moveZ(float z);
	void activate(void);
};