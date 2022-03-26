#include "dfWall.h"

#include <iostream>

const dfWallFlag flag1Implemented = (dfWallFlag)((int)dfWallFlag::MORPHS_WITH_ELEV | (int)dfWallFlag::FLIP_TEXTURE_HORIZONTALLY);
const int flag3Implemented = 65536;

static uint32_t g_ids = 1;

dfWall::dfWall(int left, int right, int adjoint, int mirror, dfWallFlag flag1, dfWallFlag flag3) :
	m_id(g_ids++),
	m_left(left),
	m_right(right),
	m_adjoint(adjoint),
	m_mirror(mirror),
	m_flag1(flag1),
	m_flag3(flag3)
{
	if (m_flag1 != dfWallFlag::ALL && (int(m_flag1) & int(flag1Implemented)) == 0) {
		std::cerr << "dfWall::dfWall flag1 = " << (int)m_flag1 << " not implemented" << std::endl;
	}
	if (m_flag3 != dfWallFlag::ALL && (int(m_flag3) & int(flag1Implemented)) == 0) {
		std::cerr << "dfWall::dfWall flag3 = " << (int)m_flag3 << " not implemented" << std::endl;
	}
}