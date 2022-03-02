#pragma once

#include <map>
#include <string>

namespace DarkForces {
	class Msg {
		uint32_t m_id=0;
		uint32_t m_importance=0;
		std::string m_text;
	public:
		Msg(void) {};
		Msg(uint32_t id, uint32_t importance, const std::string& string):
			m_id(id),
			m_importance(importance),
			m_text(string)
		{}
	};

	class Msgs {
		std::map<uint32_t, Msg> m_msgs;
	public:
		Msgs(void) {};
		void Parse(const std::string& file);
		inline const Msg& get(uint32_t id) {
			return m_msgs[id];
		}
	};
}

extern DarkForces::Msgs g_dfMsg;