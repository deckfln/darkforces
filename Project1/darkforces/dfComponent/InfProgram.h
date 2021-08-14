#pragma once

#include "../../gaEngine/gaComponent.h"
#include "../../gaEngine/gaMessage.h"
#include <vector>
#include <string>

namespace DarkForces {

	enum class Event {
		DF_TRIGGER_SWITCH1,
		DF_TRIGGER_STANDARD,
	};

	// manage events described in the INF file
	class InfProgram : public gaComponent {
		Event m_class;
		uint32_t m_eventMask = 0;
		bool m_master = true;				// is the trigger operational ?
		bool m_actived = false;				// trigger was activated and shall not accept any new activation
		std::vector<std::string> m_clients;		// name of the target sector 

		std::string m_sector;				// sector that host the trigger
		uint32_t m_wallIndex = -1;			// index of the wall being a trigger

		std::vector<gaMessage*> m_messages;	// messages to pass to the clients
		void execute(void);					// execute the program

	public:
		InfProgram(const std::string& trigger, const std::string& sector);
		inline void eventMask(uint32_t mask) { m_eventMask = mask; };
		inline void client(const std::string& client) { m_clients.push_back(client); };
		inline void message(gaMessage* message) { m_messages.push_back(message); };
		void compile(void);					// compile the event
		void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
		void debugGUIinline(void) override;					// display the component in the debugger

		~InfProgram();
	};
};