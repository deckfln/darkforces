#include "dfWall.h"

#include <iostream>

const int flag1Implemented = DF_WALL_MORPHS_WITH_ELEV;
const int flag3Implemented = 65536;

dfWall::dfWall(int left, int right, int adjoint, int mirror, int flag1, int flag3) :
	m_left(left),
	m_right(right),
	m_adjoint(adjoint),
	m_mirror(mirror),
	m_flag1(flag1),
	m_flag3(flag3)
{
	if (m_flag1 != 0 && (m_flag1 & flag1Implemented) == 0) {
		std::cerr << "dfWall::dfWall flag1 = " << m_flag1 << " not implemented" << std::endl;
	}
	if (m_flag3 != 0 && (m_flag3 & flag1Implemented) == 0) {
		std::cerr << "dfWall::dfWall flag3 = " << m_flag3 << " not implemented" << std::endl;
	}
}

dfWall::~dfWall()
{
}