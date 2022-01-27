#pragma once

#include <map>
#include <string>

#include "../../gaEngine/gaComponent.h"

namespace GameEngine {
	class Item {
		std::string m_name;
		bool m_on = false;
	public:
		Item(void) {
		}
		Item(const std::string& name) {
			m_name = name;
		}
		const std::string& name(void) {
			return m_name;
		}
		bool on(void) {
			return m_on;
		}
		void set(bool b) {
			m_on = b;
		}
	};
	namespace Component {
		class Inventory : public gaComponent
		{
			std::map<std::string, GameEngine::Item*> m_items;
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
