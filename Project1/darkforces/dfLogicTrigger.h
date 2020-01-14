#pragma once

#include <string>
#include <map>
#include <list>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "dfLogicElevator.h"

class dfSector;

enum {
	DF_MESSAGE_GOTO_STOP
};

class dfMessage {
public:
	int m_action;
	int m_value;

	dfMessage(int action, int value) { m_action = action; m_value = value; };
};

class dfLogicTrigger {
	std::string m_class;
	int m_eventMask = 0;
	std::list<std::string> m_clients;		// name of the target sector 
	std::list<dfLogicElevator*> m_pClients;  // pointer to the target sector 

	std::string m_sector;			// sector that host the trigger
	dfSector* m_pSector = nullptr;	// sector that host the trigger
	int m_wallIndex = -1;			// index of the wall being a trigger

	fwAABBox m_boundingBox;			// bouding box of the triggers
	glm::vec3 m_boundingBoxCenter;	// original position of the bounding box
	glm::vec3 m_boundingBoxSize;	// original size of the bounding box

	std::list<dfMessage> m_messages;	// messages to pass to the clients

public:
	dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex);
	dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex, dfLogicElevator *client);
	dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client);

	void eventMask(int eventMask) { m_eventMask = eventMask; };
	void client(std::string& client) { m_clients.push_back(client); }
	std::list<std::string>& clients(void) { return m_clients; };
	std::string& sector(void) { return m_sector; };
	int wall(void) { return m_wallIndex; };

	void bindSector(dfSector* pSector);
	void evelator(dfLogicElevator* pClient);
	void boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling);
	void message(std::vector <std::string>& tokens);

	bool collide(fwAABBox& box);
	void moveZ(float z);
	void activate(void);
};