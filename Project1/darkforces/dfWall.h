#include <glm/glm.hpp>
#include <string>

#define DFWALL_TEXTURE_TOP 0
#define DFWALL_TEXTURE_MID 1
#define DFWALL_TEXTURE_BOTTOM 2
#define DFWALL_TEXTURE_SIGN 3

class dfWall
{
public:
	int m_left = -1;
	int m_right = -1;
	int m_adjoint = -1;
	int m_mirror = -1;

	glm::vec3 m_tex[4];

	dfWall(int left, int right, int ajdoint, int mirror);
	~dfWall();
};