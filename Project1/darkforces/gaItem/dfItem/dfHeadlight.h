#pragma once

#include "../dfItem.h"

namespace DarkForces {
	class Headlight: public DarkForces::Item {
		bool m_on = false;
	public:
		Headlight(const std::string& name);

		inline bool on(void) {
			return m_on;
		}
		inline void set(bool b) {
			m_on = b;
		}
	};
}