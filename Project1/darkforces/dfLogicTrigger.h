#pragma once

#include <string>
#include <map>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "dfLogicElevator.h"
#include "dfMessageClient.h"

class dfSector;
class dfWall;
class dfSign;

enum {
	DF_TRIGGER_SWITCH1,
	DF_TRIGGER_STANDARD,
};

class dfLogicTrigger: public dfMessageClient {
	int m_class;
	int m_eventMask = 0;
	bool m_master = true;			// is the trigger operational ?
	bool m_actived = false;			// trigger was activated and shall not accept any new activation
	std::vector<std::string> m_clients;		// name of the target sector 

	std::string m_sector;			// sector that host the trigger
	int m_wallIndex = -1;			// index of the wall being a trigger
	dfSign* m_pSign = nullptr;		// index of the sign being a trigger

	fwAABBox m_boundingBox;			// bouding box of the triggers
	glm::vec3 m_boundingBoxCenter;	// original position of the bounding box
	glm::vec3 m_boundingBoxSize;	// original size of the bounding box

	std::vector<dfMessage> m_messages;	// messages to pass to the clients
	dfMessage m_trigger;				// trigger message

public:
	dfLogicTrigger(std::string& kind, std::string& sector);
	dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex);
	dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex, dfLogicElevator *client);
	dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client);
	dfLogicTrigger(std::string& kind, dfLogicElevator* client);

	void eventMask(int eventMask) { m_eventMask = eventMask; };
	void client(std::string& client) { m_clients.push_back(client); }
	std::vector<std::string>& clients(void) { return m_clients; };
	std::string& sector(void) { return m_sector; };
	int wall(void) { return m_wallIndex; };
	void sign(dfSign* sign) { m_pSign = sign; };
	void addEvents(dfSector* pSector);
	void evelator(dfLogicElevator* pClient);
	void boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling);
	void boundingBox(fwAABBox& box);
	void message(std::vector <std::string>& tokens);
	void config(void);

	bool collide(fwAABBox& box);
	void moveZ(float z);
	void activate(void);
	void dispatchMessage(dfMessage* message);
};