#include <glm/glm.hpp>
#include <string>

#define DFWALL_TEXTURE_TOP 0
#define DFWALL_TEXTURE_MID 1
#define DFWALL_TEXTURE_BOTTOM 2
#define DFWALL_TEXTURE_SIGN 3

class dfSector;
class dfSign;

class dfWall
{
	dfSign* m_sign = nullptr;		//  details of the sign loaded on the wall

public:
	int m_id = 0;

	int m_left = -1;				// index of the left vertice (in the sector vertices)
	int m_right = -1;
	int m_adjoint = -1;				//	index of the adjoint sector (in the level sectors)
	int m_mirror = -1;				//	index of the wall on the other side of the adjoint (in the adjoint walls)

	dfSector *m_pAdjoint = nullptr;	//	index of the adjoint sector (in the level sectors)
	dfWall *m_pMmirror = nullptr;	//	index of the wall on the other side of the adjoint (in the adjoint walls)

	glm::vec3 m_tex[4];

	dfWall(int left, int right, int ajdoint, int mirror);
	void sign(dfSign* sign) { m_sign = sign; };
	dfSign* sign(void) { return m_sign; };
	~dfWall();
};