#pragma once

#include "../gaEngine/gaEntity.h"

namespace DarkForces
{
	class Goals : public gaEntity {
		struct Goal {
			bool m_complete = false;
			uint32_t m_id;
			uint32_t m_template;
		};
		std::vector<struct Goal> m_goals;					// list of goals
		uint32_t m_completed = 0;							// completed goals
		void parse(const std::string& file);				// parse the .GOL file
		void onTrigger(gaMessage* message);					// tick goals
	public:
		Goals(const std::string& file);
		void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation
	};
}