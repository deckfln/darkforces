#pragma once
#include <string>
#include <vector>
#include <list>
#include "dfWall.h"
#include "../framework/fwMesh.h"

class dfSector
{
	std::string m_name = "";
	float m_ambient = 0;
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;
	std::vector <dfWall*> m_walls;
	std::vector <glm::vec2> m_vertices;

public:
	dfSector(std::ifstream& infile);
	~dfSector();
};