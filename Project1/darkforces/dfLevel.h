#pragma once
#include <string>
#include <vector>

#include "dfSector.h"

class dfLevel
{
	std::string m_name;
	std::string m_level;
	std::vector<dfSector *> m_sectors;

	fwGeometry* m_geometry = nullptr;
	glm::vec3* m_vertices = nullptr;
	void convert2geometry(void);

public:
	dfLevel(std::string name);
	fwGeometry* geometry(void) { return m_geometry; };
	~dfLevel();
};