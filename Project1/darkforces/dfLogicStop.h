#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>

#include "../framework/fwAABBox.h"
#include "../gaEngine/gaMessage.h"

#include "dfLogicElevatorConst.h"

class dfSector;
class dfLogicElevator;

enum {
	DF_STOP_HOLD,
	DF_STOP_COMPLETE,
	DF_STOP_TERMINATE
};

class dfLogicStop {
	std::string m_name;

	int m_flag = 0;			// content of the stop
		// 1 : absolute
		// 2 : relative
		// 4 : based on antoher sector

		// 8 : time to spend at a stop
		// 16 : action at the stop

	float m_absolute = 0;	// absolute stop of the elevator
	float m_relatiave = 0;	// relative stop from the floor

	std::string m_sector;	// based on sector XXX
	dfSector *m_pSector = nullptr;

	float m_time = 0;			// time (millisecond) to stop the elevator a position (absolute or relative or sector)

	dfLogicElevator* m_parent = nullptr;

	// [hold elevator] will remain at stop indefinitely 
	// [terminate] elevator will stay at the stop permanently 
	// [complete] mission will be complete when elev arrives at stop
	int m_action = 0;

	std::vector<gaMessage* > m_messages;

public:
	dfLogicStop(dfLogicElevator *parent);
	dfLogicStop(dfLogicElevator* parent, float altitude, dfSector* sector, std::string& action);
	dfLogicStop(dfLogicElevator* parent, float altitude, dfSector* sector, float time);
	dfLogicStop(dfLogicElevator* parent, float altitude, std::string& action);
	dfLogicStop(dfLogicElevator* parent, float altitude, float time);

	void absolute(float absolute) { m_flag |= 1; m_absolute = absolute; };
	void relative(float relative) { m_flag |= 2; m_relatiave = relative; };
	void sector(std::string& sector) { m_flag |= 4; m_sector = sector; };
	std::string& sector(void) { return m_sector; };
	void sector(dfSector* pSector);
	void time(float time) { m_flag |= 8; m_time = time * 1000; };
	float time(void) { return m_time; };
	void action(std::string& action);
	int action(void) { return m_action; };
	bool isTimeBased(void);
	void message(gaMessage* message);
	void sendMessages();
	float z_position(dfElevatorType elevatorClass);
	void getMessagesToSectors(std::list<std::string>& sectors);

	~dfLogicStop();
};