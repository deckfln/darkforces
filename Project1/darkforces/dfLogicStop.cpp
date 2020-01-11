#include "dfLogicStop.h"

#include "dfSector.h"

/**
 * return a new position based on the source position and the refernce
 */
bool dfLogicStop::isTimeBased(void)
{
	int a = m_flag & 8;  
	return a == 8;
}

float dfLogicStop::z_position(void)
{
	switch (m_flag) {
		case 9:
		case 17:
			return m_absolute;
		case 10:
		case 18:
			if (!m_pSector) {
				std::cerr << "dfLogicStop::z_position no linked sector" << std::endl;
				return 0;
			}
			return m_pSector->originalFloor() + m_relatiave;
		case 12:
		case 20:
			return m_pSector->ceiling();
	}
	return 0;
}
