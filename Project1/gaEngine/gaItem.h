#pragma once

#include <string>

#include "../gaEngine/gaMessage.h"

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

		virtual void dispatchMessage(gaMessage* message) {};
	};
}