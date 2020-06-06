#include <glm/glm.hpp>
#include <string>

#define DFWALL_TEXTURE_TOP 0
#define DFWALL_TEXTURE_MID 1
#define DFWALL_TEXTURE_BOTTOM 2
#define DFWALL_TEXTURE_SIGN 3

enum class dfWallFlag {
	// flag1
	ALL = 0,
	ADJOINING_MID_TX=1,		// the MID TX is NOT removed 
	ILLUMINATED_SIGN=2,
	FLIP_TEXTURE_HORIZONTALLY=4,
	ELEV_CAN_CHANGE_WALL_LIGHT=8,
	TX_ANCHORED=16,
	MORPHS_WITH_ELEV=32,
	ELEV_CAN_SCROLL_TOP_TX=64,
	ELEV_CAN_SCROLL_MID_TX=128,
	ELEV_CAN_SCROLL_BOT_TX=256,
	ELEV_CAN_SCROLL_SIGN_TX=512,
	HIDE_ON_MAP=1024,
	SHOW_AS_NORMAL_ON_MAP=2048,	//  i.e.light green 
	SIGN_ANCHORED=4096,
	DAMAGES_PLAYER=8192,
	SHOW_AS_LEDGE_ON_MAP=16384,	// i.e.dark green 
	SHOW_AS_DOOR_ON_MAP=32768,	// i.e.yellow

	NOT_MORPHS_WITH_ELEV = 65536,

	//flag 3
	CAN_ALWAYS_WALK=1,					// Player will climb any height 
	PLAYER_ENEMIES_CANNOT_WALK_THROUGH_WALL=2,
	ENEMIES_ONLY_CANNOT_WALK_THROUGH_WALL=4,
	CANNOT_FIRE_THROUGH_WALL=8
};

class dfSector;
class dfSign;

class dfWall
{
private:
	dfSector* m_sector = nullptr;

public:
	int m_id = 0;

	int m_left = -1;				// index of the left vertice (in the sector vertices)
	int m_right = -1;
	int m_adjoint = -1;				//	index of the adjoint sector (in the level sectors)
	int m_mirror = -1;				//	index of the wall on the other side of the adjoint (in the adjoint walls)
	dfWallFlag m_flag1 = dfWallFlag::ALL;
	dfWallFlag m_flag3 = dfWallFlag::ALL;

	dfSector *m_pAdjoint = nullptr;	//	index of the adjoint sector (in the level sectors)
	dfWall *m_pMmirror = nullptr;	//	index of the wall on the other side of the adjoint (in the adjoint walls)

	glm::vec3 m_tex[4];

	dfWall(int left, int right, int ajdoint, int mirror, dfWallFlag flag1, dfWallFlag flag3);
	bool flag1(dfWallFlag flag) { return (int)m_flag1 & (int)flag; };
	void sector(dfSector* parent) { m_sector = parent; };
	dfSector* sector(void) { return m_sector; };
	~dfWall();
};