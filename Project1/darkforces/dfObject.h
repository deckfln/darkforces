#pragma once
#include <string>

class dfModel;
class dfSprites;

class dfObject
{
protected:
	float m_x=0, m_y=0, m_z=0;
	dfModel* m_source;
public:
	dfObject(dfModel *source, float x, float y, float z);
	bool named(std::string name);
	void addToSprites(dfSprites* sprites);
	~dfObject();
};