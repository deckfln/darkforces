#include <glm/glm.hpp>
#include <string>

#define DFWALL_TEXTURE_TOP 0
#define DFWALL_TEXTURE_MID 1
#define DFWALL_TEXTURE_BOTTOM 2
#define DFWALL_TEXTURE_SIGN 3

enum {
	DF_WALL_ALL = 0,
	DF_WALL_ADJOINING_MID_TX=1,		// the MID TX is NOT removed 
	DF_WALL_ILLUMINATED_SIGN=2,
	DF_WALL_FLIP_TEXTURE_HORIZONTALLY=4,
	DF_WALL_ELEV_CAN_CHANGE_WALL_LIGHT=8,
	DF_WALL_TX_ANCHORED=16,
	DF_WALL_MORPHS_WITH_ELEV=32,
	DF_WALL_ELEV_CAN_SCROLL_TOP_TX=64,
	DF_WALL_ELEV_CAN_SCROLL_MID_TX=128,
	DF_WALL_ELEV_CAN_SCROLL_BOT_TX=256,
	DF_WALL_ELEV_CAN_SCROLL_SIGN_TX=512,
	DF_WALL_HIDE_ON_MAP=1024,
	DF_WALL_SHOW_AS_NORMAL_ON_MAP=2048,	//  i.e.light green 
	DF_WALL_SIGN_ANCHORED=4096,
	DF_WALL_DAMAGES_PLAYER=8192,
	DF_WALL_SHOW_AS_LEDGE_ON_MAP=16384,	// i.e.dark green 
	DF_WALL_SHOW_AS_DOOR_ON_MAP=32768,	// i.e.yellow

	DF_WALL_NOT_MORPHS_WITH_ELEV = 65536,
};

enum {
	DF_WALL_CAN_ALWAYS_WALK=1,					// Player will climb any height 
	DF_WALL_PLAYER_ENEMIES_CANNOT_WALK_THROUGH_WALL=2,
	DF_WALL_ENEMIES_ONLY_CANNOT_WALK_THROUGH_WALL=4,
	DF_WALL_CANNOT_FIRE_THROUGH_WALL=8
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
	int m_flag1 = 0;
	int m_flag3 = 0;

	dfSector *m_pAdjoint = nullptr;	//	index of the adjoint sector (in the level sectors)
	dfWall *m_pMmirror = nullptr;	//	index of the wall on the other side of the adjoint (in the adjoint walls)

	glm::vec3 m_tex[4];

	dfWall(int left, int right, int ajdoint, int mirror, int flag1, int flag3);
	int flag1(int flag) { return m_flag1 & flag; };
	void sector(dfSector* parent) { m_sector = parent; };
	dfSector* sector(void) { return m_sector; };
	~dfWall();
};