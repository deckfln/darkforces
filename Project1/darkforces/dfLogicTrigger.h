#pragma once

#include <string>
#include <map>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "../gaEngine/gaEntity.h"
#include "../gaEngine/World.h"

#include "dfComponent/dfComponentActor.h"
#include "flightRecorder/frLogicTrigger.h"
#include "../config.h"

class dfSector;
class dfWall;
class dfSign;
class dfMesh;

enum {
	DF_TRIGGER_SWITCH1,
	DF_TRIGGER_STANDARD,
};

class dfLogicTrigger: public gaEntity {
	int m_class;
	int m_eventMask = 0;
	bool m_master = true;					// is the trigger operational ?
	bool m_actived = false;					// trigger was activated and shall not accept any new activation
	std::vector<std::string> m_clients;		// name of the target sector 
	uint32_t m_keys = DarkForces::Keys::NONE;		// keys needed to activate the triggers

	std::string m_sector;					// sector that host the trigger
	int m_wallIndex = -1;					// index of the wall being a trigger

	dfMesh* m_pMesh= nullptr;				// Mesh being a trigger (has a bounding box)

	std::vector<gaMessage *> m_messages;	// messages to pass to the clients
	gaMessage m_trigger;					// trigger message

	void init(const std::string& kind);

public:
	dfLogicTrigger(const std::string& kind, const std::string& sector);
	dfLogicTrigger(const std::string& kind, const std::string& sector, int wallIndex);

	// getter/setter
	inline void eventMask(int eventMask) { m_eventMask = eventMask; };
	inline void client(std::string& client) { m_clients.push_back(client); }
	inline std::vector<std::string>& clients(void) { return m_clients; };
	inline const std::string& sector(void) { return m_sector; };
	inline int wall(void) { return m_wallIndex; };
	inline void sign(dfSign* _sign) { m_pMesh = (dfMesh *)_sign; };
	inline void keys(int keys) { m_keys = keys; };
	void addEvents(dfSector* pSector);
	inline std::vector<gaMessage*>& messages(void) { return m_messages; };	// return messages

	void boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling);
	void boundingBox(fwAABBox& box);

	void message(gaMessage *message);
	void config(void);

	void moveZ(float z);
	void moveCeiling(float z);
	void activate(const std::string& activor);
	void dispatchMessage(gaMessage* message);

	// flight recorder
	int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::LogicTrigger);
	};														// size of one record

	uint32_t recordState(void* record) override;			// return a record of an actor state (for debug)
	void loadState(void* record) override;					// reload an actor state from a record
	void debugGUIChildClass(void) override;					// Add dedicated component debug the entity

	~dfLogicTrigger();
};