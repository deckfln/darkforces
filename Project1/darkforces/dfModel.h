#pragma once

#include <string>

class dfModel
{
protected:
	std::string m_name;
public:
	dfModel(std::string& name);
	bool named(std::string& name);
	virtual int textureID(void) { return 0; };
};