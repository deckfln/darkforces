#pragma once

#include <string>

#include "../gaEngine/gaMessage.h"

namespace GameEngine {
	namespace Component {
		class Inventory;
	};

	class Item {
		std::string m_name;

	protected:
		GameEngine::Component::Inventory* m_inventory = nullptr;

	public:
		Item(void) {
		}
		Item(const std::string& name) {
			m_name = name;
		}
		inline const std::string& name(void) {
			return m_name;
		}
		inline void set(const char* name) {
			m_name = name;
		}

		// link back to inventory
		inline void inventory(GameEngine::Component::Inventory* inventory) {
			m_inventory = inventory;
		}

		virtual void dispatchMessage(gaMessage* message) {};

#ifdef _DEBUG
		virtual void debugGUIinline(void) {};
#endif
	};
}