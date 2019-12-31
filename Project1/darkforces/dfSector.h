#pragma once
#include <string>
#include <vector>
#include <list>
#include "dfWall.h"
#include "../framework/fwMesh.h"
#include "../framework/fwAABBox.h"

class dfSector
{
public:
	fwAABBox m_boundingBox;

	std::string m_name = "";
	int m_id = -1;
	int m_layer = -1;
	float m_ambient = 0;
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;
	std::vector <dfWall*> m_walls;
	std::vector <glm::vec2> m_vertices;

	dfSector(std::ifstream& infile);
	bool inAABBox(glm::vec3& position) { return m_boundingBox.inside(position); };
	bool isPointInside(glm::vec3& position);
	~dfSector();
};