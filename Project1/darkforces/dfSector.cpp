#include "dfSector.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#include <algorithm>
#include <glm/glm.hpp>

#include "dfSuperSector.h"
#include "dfMesh.h"
#include "dfParseINF.h"
#include "dfMessageBus.h"

dfSector::dfSector(std::istringstream& infile)
{
	int nbVertices;
	int currentVertice = 0;

	int nbWalls;
	int currentWall = 0;

	std::string line, dump;
	float min_x=99999, max_x=-99999, min_y=99999, max_y=-99999;	// build AABBox while parsing the vertices

	// per line
	while (std::getline(infile, line))
	{
		// end of sector
		if (line.find("S e c t o r   D e f i n i t i o n") != std::string::npos) {
			break;
		}

		// ignore comment
		if (line[0] == '#' || line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "NAME" && tokens.size() > 1) {
			m_name = tokens[1];
		}
		if (tokens[0] == "LAYER") {
			m_layer = std::stoi(tokens[1]);
		}
		else if (tokens[0] == "AMBIENT") {
			m_ambient = std::stof(tokens[1]);
		}
		else if (tokens[0] == "FLOOR") {
			if (tokens[1] == "TEXTURE") {
				m_floorTexture = glm::vec3(
					std::stof(tokens[2]),	// textureID
					std::stof(tokens[3]),	// texture xoffset
					-std::stof(tokens[4])	// texture yoffset
				);
			}
			else if (tokens[1] == "ALTITUDE") {
				m_floorAltitude = -std::stof(tokens[2]);
			}
		}
		else if (tokens[0] == "CEILING") {
			if (tokens[1] == "TEXTURE") {
				m_ceilingTexture = glm::vec3(
					std::stof(tokens[2]),	// textureID
					std::stof(tokens[3]),	// texture xoffset
					-std::stof(tokens[4])	// texture yoffset
				);
			}
			else if (tokens[1] == "ALTITUDE") {
				m_ceilingAltitude = -std::stof(tokens[2]);
			}
		}
		else if (tokens[0] == "SECOND") {
			if (tokens[1] == "ALTITUDE") {
				//PASS
			}
		}
		else if (tokens[0] == "FLAGS") {
			m_flag1 = std::stoi(tokens[1]);
			m_flag2 = std::stoi(tokens[2]);
			m_flag3 = std::stoi(tokens[3]);
		}
		else if (tokens[0] == "VERTICES") {
			nbVertices = std::stoi(tokens[1]);
			m_vertices.resize(nbVertices);
			m_verticeConnexions.resize(nbVertices);
		}
		else if (tokens[0] == "X:") {
			float x = -std::stof(tokens[1]);
			float z = std::stof(tokens[3]);

			m_vertices[currentVertice++] = glm::vec2(x, z);

			// refresh the AABBox
			if (x < min_x) min_x = x;
			if (x > max_x) max_x = x;
			if (z < min_y) min_y = z;
			if (z > max_y) max_y = z;
		}
		else if (tokens[0] == "WALLS") {
			nbWalls = std::stoi(tokens[1]);
			m_walls.resize(nbWalls);
		}
		else if (tokens[0] == "WALL") {
			int left = std::stoi(tokens[2]);
			int right = std::stoi(tokens[4]);
			int adjoint = std::stoi(tokens[25]);
			int mirror = std::stoi(tokens[27]);
			glm::vec3 mid(std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8]));
			glm::vec3 top(std::stof(tokens[11]), std::stof(tokens[12]), std::stof(tokens[13]));
			glm::vec3 bottom(std::stof(tokens[16]), std::stof(tokens[17]), std::stof(tokens[18]));
			glm::vec3 sign(std::stof(tokens[21]), std::stof(tokens[22]), std::stof(tokens[23]));

			dfWall* wall = new dfWall(left, right, adjoint, mirror);
			wall->m_tex[DFWALL_TEXTURE_BOTTOM] = bottom;
			wall->m_tex[DFWALL_TEXTURE_MID] = mid;
			wall->m_tex[DFWALL_TEXTURE_TOP] = top;
			wall->m_tex[DFWALL_TEXTURE_SIGN] = sign;
			wall->m_id = currentWall;

			m_walls[currentWall++] = wall;

			if (adjoint >= 0) {
				m_portals.push_back(adjoint);
			}
		}
	}

	m_height = m_ceilingAltitude - m_floorAltitude;
	m_boundingBox = fwAABBox(min_x, max_x, min_y, max_y, m_floorAltitude, m_ceilingAltitude);

	m_message = dfMessage(DF_MESSAGE_TRIGGER, 0, m_name);
}

/**
 * configure the trigger based on the wall of the sector
 */
void dfSector::setTriggerFromWall(dfLogicTrigger* trigger)
{
	unsigned int wallID = trigger->wall();

	if (wallID < m_walls.size()) {
		// create a bounding box for the trigger. Based off the wall
		dfWall* wall = m_walls[wallID];

		trigger->boundingBox(
			m_vertices[wall->m_left], m_vertices[wall->m_right],
			m_floorAltitude, m_ceilingAltitude
			);
	}
	m_triggers.push_back(trigger);
}

/**
 * configure the trigger based on the wall of the sector
 */
void dfSector::setTriggerFromSector(dfLogicTrigger* trigger)
{
	trigger->boundingBox(m_boundingBox);
	m_triggers.push_back(trigger);
}

/**
 * configure the trigger based on the floor of the sector
 */
void dfSector::setTriggerFromFloor(dfLogicTrigger* trigger)
{
	glm::vec2 left(m_boundingBox.m_x, m_boundingBox.m_y);
	glm::vec2 right(m_boundingBox.m_x1, m_boundingBox.m_y1);

	trigger->boundingBox(
		left, right,
		m_floorAltitude, m_floorAltitude
	);

	m_triggers.push_back(trigger);
}

/**
 * Move the floor of the sector
 * Also move all triggers on the sector
 */
void dfSector::floor(float z)
{
	m_floorAltitude = z;

	for (auto trigger : m_triggers) {
		trigger->moveZ(z);
	}
}

/**
 * Move the floor of the sector
 * Also move all triggers on the sector
 */
void dfSector::ceiling(float z)
{
	m_ceilingAltitude = z;

	if (m_triggers.size() > 0) {
		std::cerr << "dfSector::ceiling triggers not implemented" << std::endl;
	}
}

/**
 * Return a list of walls based on the sector status
 *	list of ALL walls
 *	list of the external walls
 */
std::vector<dfWall*>& dfSector::walls(int displayPolygon)
{
	static std::vector<dfWall*> ml;

	int dp = (displayPolygon == -1) ? m_displayPolygons : displayPolygon;

	assert(dp >= 0 && dp < 3);

	switch (dp) {
	case 0:
		return m_walls;	// all walls
	case 1:
		return m_polygons_walls[0];	// only the external polygon
	case 2:
		return m_polygons_walls[1];	// only the hole polygon
	}

	return ml;
}

/**
 * Return the polygon(s) makeing the sector, based on the number of polygons to display
 */
std::vector<std::vector<Point>>& dfSector::polygons(int displayPolygon)
{
	static std::vector<std::vector<Point>> ml;

	int dp = (displayPolygon == -1) ? m_displayPolygons : displayPolygon;

	assert(dp >= 0 && dp < 3);

	switch (dp) {
	case 0:
		return m_polygons_vertices;	// all walls
	case 1:
		ml.clear();
		ml.push_back(m_polygons_vertices[0]);
		return ml;	// only the external polygon
	case 2:
		ml.clear();
		ml.push_back(m_polygons_vertices[1]);
		return ml;	// only the hole polygon
	}

	ml.clear();
	return ml;
}

/**
 * Add a mesh to the super sector holding the sector
 */
void dfSector::addObject(fwMesh* object)
{
	m_super->addObject(object);
}

/**
 * check if point is inside the boundingbox and inside the 2D surface : external polylines
 * TODO : how to deal with holes ?
 */
bool dfSector::isPointInside(glm::vec3 &p, bool fullTest)
{
	// quick check against the 3D bounding box
	if (!m_boundingBox.inside(p)) {
		return false;
	}

	// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
	// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	bool inside = false;

	// test holes
	if (fullTest) {
		for (unsigned int i = 1; i < m_polygons_vertices.size(); i++) {
			std::vector<Point>& line = m_polygons_vertices[i];

			for (unsigned int i = 0, j = line.size() - 1; i < line.size(); j = i++)
			{
				if ((line[i][1] > p.y) != (line[j][1] > p.y) &&
					p.x < (line[j][0] - line[i][0]) * (p.y - line[i][1]) / (line[j][1] - line[i][1]) + line[i][0])
				{
					inside = !inside;
				}

				if (inside) {
					return false;	// if we are in the hole, we are not on the sector
				}
			}

		}
	}

	/*
	// TODO: these solution is supposed to deal with holes, but actually it doesn't work for secbase, when entering sector 58
	for (unsigned int i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++)
	{
		if ((m_vertices[i].y > p.y) != (m_vertices[j].y > p.y) &&
			p.x < (m_vertices[j].x - m_vertices[i].x) * (p.y - m_vertices[i].y) / (m_vertices[j].y - m_vertices[i].y) + m_vertices[i].x)
		{
			inside = !inside;
		}
	}
	*/

	// finish with the external line
	std::vector<Point>& outline = m_polygons_vertices[0];
	for (unsigned int i = 0, j = outline.size() - 1; i < outline.size(); j = i++)
	{
		if ((outline[i][1] > p.y) != (outline[j][1] > p.y) &&
			p.x < (outline[j][0] - outline[i][0]) * (p.y - outline[i][1]) / (outline[j][1] - outline[i][1]) + outline[i][0])
		{
			inside = !inside;
		}
	}

	return inside;
}

/**
 * return the surface of the sector bounding box
 */
float dfSector::boundingBoxSurface(void)
{
	return m_boundingBox.surface();
}

/**
 * Change the sector floor & ceiling altitudes IN the super-sector openGL vertices
 */
void dfSector::setFloor(float floor)
{
	m_ceilingAltitude = floor + m_height;
	m_floorAltitude = floor;

	if (m_super) {
		m_super->updateSectorVertices(m_id);	// update the sector

		// update the connected portals
		for (auto portal : m_portals) {
			m_super->updateSectorVertices(portal);
		}
	}
}

/**
 * Move the sector up or down. return the new floor altitude
 */
void dfSector::updateVertices(void)
{
	// std::cout << m_floorAltitude << " " << m_ceilingAltitude << std::endl;
	if (m_super) {
		m_super->updateSectorVertices(m_id);	// update the sector

		// update the connected portals
		for (auto portal : m_portals) {
			m_super->updateSectorVertices(portal);
		}
	}
}

/**
 * parse all vertices of the sector to link the walls together
 */
void dfSector::linkWalls(void)
{
	for (auto wall : m_walls) {
		m_verticeConnexions[wall->m_left].m_rightVertice = wall->m_right;
		m_verticeConnexions[wall->m_right].m_leftVertice = wall->m_left;

		m_verticeConnexions[wall->m_left].m_leftWall = wall->m_id;
		m_verticeConnexions[wall->m_right].m_rightWall = wall->m_id;
	}

	int links = 0;
	for (unsigned int start = 0; start < m_verticeConnexions.size(); start++) {
		if (!m_verticeConnexions[start].parsed) {
			std::vector<Point> polygon;
			std::vector<dfWall*> polygonWalls;

			// vertice was not yet checked, so start a link
			unsigned int i;

			for (i = start; 
					m_verticeConnexions[i].m_rightVertice != start &&		// detect loop
					m_verticeConnexions[i].m_rightVertice != -1 &&			// detect open lines
					!m_verticeConnexions[i].parsed;					// detect point used multiple times
				i = m_verticeConnexions[i].m_rightVertice					// move to next vertice
				) 
			{
				m_verticeConnexions[i].parsed = true;
				Point p = { m_vertices[i].x, m_vertices[i].y };
				polygon.push_back(p);

				if (m_verticeConnexions[i].m_leftWall >= 0) {
					polygonWalls.push_back(m_walls[m_verticeConnexions[i].m_leftWall]);
				}
			}
			Point p = { m_vertices[i].x, m_vertices[i].y };
			polygon.push_back(p);
			if (m_verticeConnexions[i].m_leftWall >= 0) {
				polygonWalls.push_back(m_walls[m_verticeConnexions[i].m_leftWall]);
			}

			m_verticeConnexions[i].parsed = true;

			// only create polygons for closed line and at least 3 vertices
			if (polygon.size() >= 3 && m_verticeConnexions[i].m_rightVertice != -1) {
				m_polygons_vertices.push_back(polygon);
				m_polygons_walls.push_back(polygonWalls);
			}
		}
	}

	// find what is the perimeter and what is the hole
	// TODO deal with more than 1 hole
	if (m_polygons_vertices.size() > 1) {

		if (m_polygons_vertices.size() > 2) {
			std::cerr << "dfSector::linkWalls sector with more than 1 hole is not implemented" << std::endl;
		}

		std::vector<Point>& polygon = m_polygons_vertices[1];
		Point p = m_polygons_vertices[0][1];

		// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
		// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
		bool inside = false;
		for (unsigned int i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++)
		{
			if ((polygon[i][1] > p[1]) != (polygon[j][1] > p[1]) &&
				p[0] < (polygon[j][0] - polygon[i][0]) * (p[1] - polygon[i][1]) / (polygon[j][1] - polygon[i][1]) + polygon[i][0])
			{
				inside = !inside;
			}
		}

		if (inside) {
			std::vector<Point> swap = m_polygons_vertices[0];
			m_polygons_vertices[0] = m_polygons_vertices[1];
			m_polygons_vertices[1] = swap;

			std::vector<dfWall *> swap1 = m_polygons_walls[0];
			m_polygons_walls[0] = m_polygons_walls[1];
			m_polygons_walls[1] = swap1;
		}
	}
}

/**
 * analyze the sector to find the moveable part and convert to an object
 */
void dfSector::buildElevator(dfMesh *mesh, float bottom, float top, int what, bool clockwise, int displayPolygon)
{
	if (!m_super) {
		return;
	}

	std::vector<dfBitmap *>& textures = m_super->textures();

	// create the walls
	std::vector <dfWall*>& wallss = walls(displayPolygon);
	for (auto wall : wallss) {
		if (wall->m_adjoint < 0) {
			// full wall (for spin1 elevators)
			mesh->addRectangle(this, wall,
				bottom,
				top,
				wall->m_tex[what],
				textures,
				true
			);
			// PASS
		}
		else {
			// portal
			dfWall* mirror = wall->m_pMmirror;

			// add a wall above the portal
			mesh->addRectangle(this, wall,
				bottom,
				top,
				mirror->m_tex[what],
				textures,
				false
			);
		}
	}

	if (displayPolygon < 2) {
		// only build build top and bottom for vertical elevators (sliding ones : spin1 are not needed)
		// index the indexes IN the polyines of polygon 
		std::vector<Point> vertices;

		for (auto poly : polygons(displayPolygon)) {
			for (auto p : poly) {
				vertices.push_back(p);
			}
		}

		mesh->addFloor(vertices, polygons(displayPolygon), bottom, m_ceilingTexture, textures, clockwise);
		mesh->addFloor(vertices, polygons(displayPolygon), top, m_floorTexture, textures, clockwise);
	}
}

/**
 * Build only the floor of the sector, upward
 */
void dfSector::buildFloor(dfMesh* mesh)
{
	if (!m_super) {
		return;
	}
	std::vector<dfBitmap*>& textures = m_super->textures();
	// build top and bottom
	// index the indexes IN the polyines of polygon 
	std::vector<Point> vertices;

	for (auto poly : m_polygons_vertices) {
		for (auto p : poly) {
			vertices.push_back(p);
		}
	}

	mesh->addFloor(vertices, m_polygons_vertices, 0, m_floorTexture, textures, false);
}

/**
 * bind each wall with an adjoint to the other sector
 */
void dfSector::bindWall2Sector(std::vector<dfSector*> sectors)
{
	for (auto wall : m_walls) {
		if (wall->m_adjoint >= 0) {
			wall->m_pAdjoint = sectors[wall->m_adjoint];
			wall->m_pMmirror = wall->m_pAdjoint->m_walls[wall->m_mirror];
		}
	}
}

/**
 * Manage events sent to the sector
 */
void dfSector::event(int event_mask)
{
	if (m_eventMask & event_mask) {
		g_MessageBus.push(&m_message);
	}
}

/**
 * only keep walls that are on the perimeters
 * hard walls are moved to an elevator mesh
 */
void dfSector::removeHollowWalls(void)
{
	// the list of walls will be the external ring
	//m_walls = m_polygons_walls[0];
	m_displayPolygons = 1;
}

dfSector::~dfSector()
{
	for (auto wall: m_walls) {
		delete wall;
	}
}