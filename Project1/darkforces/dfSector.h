#pragma once
#include <string>
#include <vector>
#include <list>
#include "dfWall.h"
#include "../framework/fwMesh.h"
#include "../framework/fwAABBox.h"
#include "../framework/math/fwSphere.h"

#include "dfLogicTrigger.h"

class dfSuperSector;

struct dfWallsLink {
	int m_left = -1;	// index of the vertice on the left
	int m_right = -1;
	bool parsed = false;
};

using Coord = float;
using Point = std::array<Coord, 2>;

class dfSector
{
	std::list <dfLogicTrigger*> m_triggers;

public:
	fwAABBox m_boundingBox;

	std::string m_name = "";
	int m_id = -1;
	int m_layer = -1;
	float m_ambient = 0;

	// current values
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;

	// original values
	float m_originalFloor;
	float m_originalceiling;
	float m_height = 0;				// height of the sector

	glm::vec3 m_floorTexture;
	glm::vec3 m_ceilingTexture;

	// local data in space world
	std::vector <dfWall*> m_walls;
	std::vector <glm::vec2> m_vertices;
	std::vector <struct dfWallsLink> m_wallsLink;

	std::list <int> m_portals;	// sectorID of the portals

	// same data but in the supersector (opengl space)
	dfSuperSector* m_super = nullptr;

	dfSector(std::ifstream& infile);
	void addTrigger(dfLogicTrigger* trigger);
	void testTriggers(fwAABBox& box);
	bool inAABBox(glm::vec3& position) { return m_boundingBox.inside(position); };
	float ceiling(void) { return m_ceilingAltitude; };
	bool isPointInside(glm::vec3& position);
	float boundingBoxSurface(void);
	void setFloor(float floor);
	float moveFloor(float delta);
	float originalFloor(void) { return m_originalFloor; };
	float originalCeiling(void) { return m_originalceiling; };
	void parent(dfSuperSector* parent) { m_super = parent; };
	float height(void) { return m_height; };
	std::vector<std::vector<Point>> linkWalls(void);
	~dfSector();
};