#pragma once

#include <map>
#include <string>

namespace DarkForces {
	class Msg {
		uint32_t m_id=0;
		int32_t m_importance=0;
		std::string m_text;
	public:
		Msg(void) {};
		Msg(uint32_t id, uint32_t importance, const std::string& string):
			m_id(id),
			m_importance(importance),
			m_text(string)
		{}
		inline std::string& text(void) { return m_text; };
		inline int32_t importance(void) { return m_importance; };

		enum Logic {
			// general
			SHIELD = 114,
			BATTERY = 211,
			CLEATS = 304,
			GOGGLES = 303,
			MASK = 305,
			MEDKIT = 311,
			// weapons
			RIFLE = 100,
			RIFLE_AMO = 101,
			AUTOGUN = 103,
			AUTOGUN_AMO = 104,
			FUSION = 107,
			FUSION_AMO = 108,
			MORTAR = 105,
			MORTAR_AMO = 106,
			CONCUSSION = 110,
			CONCUSSION_AMO = 111,
			CANNON = 112,
			CANNON_AMO = 113,
			//Ammo
			ENERGY = 200,
			DETONATOR = 203,
			DETONATORS = 204,
			POWER = 201,
			MINE = 207,
			MINES = 208,
			SHELL = 205,
			SHELLS = 206,
			PLASMA = 202,
			MISSILE = 209,
			MISSILES = 210,
			// Bonus
			SUPERCHARGE = 307,
			INVICIBLE = 306,
			LIFE = 310,
			REVIVE = 308,
			// keys
			BLUE = 302,
			RED = 300,
			YELLOW = 301,
			CODE1 = 501,
			CODE2 = 502,
			CODE3 = 503,
			CODE4 = 504,
			CODE5 = 505,
			CODE6 = 506,
			CODE7 = 507,
			CODE8 = 508,
			CODE9 = 509,
			// goals
			DATATAPE = 406,
			PLANS = 400,
			DT_WEAPON = 405,
			NAVA = 402,
			PHRIK = 401,
			PILE = 312
		};
	};

	class Msgs {
		std::map<uint32_t, Msg> m_msgs;
	public:
		Msgs(void) {};
		void Parse(const std::string& file);
		inline Msg& get(uint32_t id) {
			return m_msgs[id];
		}
	};
}

extern DarkForces::Msgs g_dfMsg;

