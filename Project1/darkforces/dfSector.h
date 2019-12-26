#pragma once
#include <string>
#include <vector>
#include <list>
#include "dfWall.h"
#include "../framework/fwMesh.h"

class dfSector
{
public:
	std::string m_name = "";
	int m_layer = -1;
	float m_ambient = 0;
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;
	std::vector <dfWall*> m_walls;
	std::vector <glm::vec2> m_vertices;

	dfSector(std::ifstream& infile);
	~dfSector();
};