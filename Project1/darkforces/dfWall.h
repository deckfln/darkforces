class dfWall
{
public:
	int m_left = -1;
	int m_right = -1;
	int m_adjoint = -1;
	int m_mirror = -1;

	dfWall(int left, int right, int ajdoint, int mirror);
	~dfWall();
};