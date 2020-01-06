#pragma once
#include <string>
#include <vector>
#include <list>
#include "dfWall.h"
#include "../framework/fwMesh.h"
#include "../framework/fwAABBox.h"
#include "../framework/math/fwSphere.h"

#include "dfLogicTrigger.h"

class dfSector
{
	std::list <dfLogicTrigger*> m_triggers;

public:
	fwAABBox m_boundingBox;

	std::string m_name = "";
	int m_id = -1;
	int m_layer = -1;
	float m_ambient = 0;
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;
	glm::vec3 m_floorTexture;
	glm::vec3 m_ceilingTexture;

	// local data in space world
	std::vector <dfWall*> m_walls;
	std::vector <glm::vec2> m_vertices;
	
	// same data but in the supersector (opengl space)
	int m_positionInSuperSector = 0;	// index of the sector vertices in the supersector data

	dfSector(std::ifstream& infile);
	void addTrigger(dfLogicTrigger* trigger);
	void testTriggers(fwAABBox& box);
	bool inAABBox(glm::vec3& position) { return m_boundingBox.inside(position); };

	bool isPointInside(glm::vec3& position);
	float boundingBoxSurface(void);
	~dfSector();
};