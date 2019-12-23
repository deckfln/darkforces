#pragma once
#include <string>
#include <list>
#include "../framework/fwMesh.h"

class dfSector
{
	std::string m_name = "";
	float m_ambient = 0;
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;

	fwMesh m_mesh;

public:
	dfSector(std::fstream& infile);
	~dfSector();
};