#include "dfLogicStop.h"

#include "dfSector.h"

/**
 * coonect the Stop to a sector (for relative altitude, or another sector altitude)
 */
void dfLogicStop::sector(dfSector* pSector)
{
	if (m_flag & 2) {
		m_pSector = pSector;
	}
}

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
			return m_pSector->m_floorAltitude + m_relatiave;	// relative to the floor of the source sector
		case 12:
		case 20:
			return m_pSector->m_floorAltitude;	// coy the floor of another sector
	}
	return 0;
}
