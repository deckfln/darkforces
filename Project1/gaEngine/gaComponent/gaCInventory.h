#pragma once

#include <map>
#include <string>

#include "../gaItem.h"
#include "../../gaEngine/gaComponent.h"

namespace GameEngine {
	namespace Component {
		class Inventory : public gaComponent
		{
			std::map<std::string, GameEngine::Item*> m_items;

			void onAddItem(gaMessage* message);
			void onDropItem(gaMessage* message);

		public:
			Inventory(void);

			void add(Item* item);
			bool isPresent(const std::string& name);
			Item* get(const std::string&);

			void dispatchMessage(gaMessage* message) override;	// deal with messages

			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}
