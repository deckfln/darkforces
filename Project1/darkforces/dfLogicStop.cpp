#include "dfLogicStop.h"

#include "dfSector.h"

float dfLogicStop::z_position(void)
{
	switch (m_flag) {
		case 9:
		case 17:
			return m_absolute;
		case 10:
		case 18:
			return m_relatiave;
		case 12:
		case 20:
			return m_pSector->ceiling();
	}
	return 0;
}
