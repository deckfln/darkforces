#pragma once

#include <string>
#include <map>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaWorld.h"

#include "dfComponent/dfComponentActor.h"
#include "dfLogicElevator.h"

class dfSector;
class dfWall;
class dfSign;
class dfMesh;
class dfLogicElevator;

enum {
	DF_TRIGGER_SWITCH1,
	DF_TRIGGER_STANDARD,
};

class dfLogicTrigger: public gaEntity {
	int m_class;
	int m_eventMask = 0;
	bool m_master = true;			// is the trigger operational ?
	bool m_actived = false;			// trigger was activated and shall not accept any new activation
	std::vector<std::string> m_clients;		// name of the target sector 
	int m_keys = DF_KEY_NONE;		// keys needed to activate the triggers

	std::string m_sector;			// sector that host the trigger
	int m_wallIndex = -1;			// index of the wall being a trigger

	dfMesh* m_pMesh= nullptr;		// Mesh being a trigger (has a bounding box)
	dfLogicElevator* m_pElevator= nullptr;		// Mesh being a trigger (has a bounding box)

	//fwAABBox m_boundingBox;			// bouding box of the triggers
	//glm::vec3 m_boundingBoxCenter;	// original position of the bounding box
	//glm::vec3 m_boundingBoxSize;	// original size of the bounding box

	std::vector<gaMessage *> m_messages;	// messages to pass to the clients
	gaMessage m_trigger;				// trigger message

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
	void sign(dfSign* _sign) { m_pMesh = (dfMesh *)_sign; };
	void keys(int keys) { m_keys = keys; };
	void addEvents(dfSector* pSector);

	void boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling);
	void boundingBox(fwAABBox& box);
	void boundingBox(dfLogicElevator* elevator);

	void message(gaMessage *message);
	void config(void);

	void moveZ(float z);
	void moveCeiling(float z);
	void activate(const std::string& activor);
	void dispatchMessage(gaMessage* message);

	void elevator(dfLogicElevator* elevator);

	std::vector<gaMessage *>& messages(void) { return m_messages; };	// return messages

	~dfLogicTrigger();
};