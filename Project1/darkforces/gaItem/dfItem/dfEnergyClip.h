#pragma once

#include "../dfItem.h"

namespace DarkForces
{
	class EnergyClip : public DarkForces::Item {
		uint32_t m_energy = 0;
		uint32_t m_maxEnergy = 200;
	public:
		EnergyClip(void);

		inline uint32_t decreaseEnergy(void) {
			if (m_energy > 0) {
				m_energy--;
			}
			return m_energy;
		}
		inline uint32_t addEnergy(uint32_t value) {
			m_energy += value;
			if (m_energy > m_maxEnergy) {
				m_energy = m_maxEnergy;
			}
			return m_energy;
		}
		inline uint32_t energy(void) {
			return m_energy;
		}
		inline void maxEnergy(uint32_t energy) {
			m_maxEnergy = energy;
		}

#ifdef _DEBUG
		void debugGUIinline(void) override;
#endif
	};
}