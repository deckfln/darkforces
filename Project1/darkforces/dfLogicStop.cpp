#include "dfLogicStop.h"

#include "dfSector.h"

/**
 * return a new position based on the source position and the refernce
 */
float dfLogicStop::z_position(float altitude)
{
	switch (m_flag) {
		case 9:
		case 17:
			return m_absolute;
		case 10:
		case 18:
			return altitude + m_relatiave;
		case 12:
		case 20:
			return m_pSector->ceiling();
	}
	return 0;
}
