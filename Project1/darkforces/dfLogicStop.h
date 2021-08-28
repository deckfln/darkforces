#pragma once
#include <string>
#include <list>
#include <vector>
#include <map>

#include "../framework/fwAABBox.h"
#include "../gaEngine/gaMessage.h"

#include "dfComponent/InfElevator.h"

class dfSector;

namespace DarkForces {
	namespace Component {
		class InfElevator;
	}
}

class dfLogicStop {
public:
	enum class Action {
		HOLD,
		COMPLETE,
		TERMINATE
	};

private:
	std::string m_name;

	enum Flag : uint32_t {
		Absolute = 1,
		Relative = 2,
		BasedOnSector = 4,
		TimeAtStop = 8,
		ActionAtStop = 16
	};

	uint32_t m_flag = 0;			// content of the stop
		// 1 : absolute
		// 2 : relative
		// 4 : based on another sector

		// 8 : time to spend at a stop
		// 16 : action at the stop

	float m_absolute = 0;	// absolute stop of the elevator
	float m_relatiave = 0;	// relative stop from the floor

	std::string m_sector;	// based on sector XXX
	dfSector *m_pSector = nullptr;

	float m_animation_time = 0;			// time (millisecond) to stop the elevator a position (absolute or relative or sector)

	// [hold elevator] will remain at stop indefinitely 
	// [terminate] elevator will stay at the stop permanently 
	// [complete] mission will be complete when elev arrives at stop
	dfLogicStop::Action m_action = dfLogicStop::Action::HOLD;

	std::vector<gaMessage* > m_messages;

public:
	dfLogicStop(dfSector* sector, float altitude, const std::string& action);
	dfLogicStop(dfSector* sector, float altitude, float time);
	dfLogicStop(const std::string& sector);
	dfLogicStop(dfSector* sector);

	void absolute(float absolute) { m_flag |= 1; m_absolute = absolute; };
	void relative(float relative) { m_flag |= 2; m_relatiave = relative; };
	void sector(std::string& sector) { m_flag |= 4; m_sector = sector; };
	const std::string& sector(void) { return m_sector; };
	void sector(dfSector* pSector);
	void time(float time) { m_flag |= 8; m_animation_time = time * 1000; };
	float time(void) { return m_animation_time; };
	void action(const std::string& action);
	dfLogicStop::Action action(void) { return m_action; };
	bool isTimeBased(void);
	void message(gaMessage* message);
	void sendMessages();
	float z_position(DarkForces::Component::InfElevator::Type elevatorClass);
	void getMessagesToSectors(std::list<std::string>& sectors);

	~dfLogicStop();
};