#include "dfSector.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#include <algorithm>
#include <glm/glm.hpp>

#include "dfSuperSector.h"

dfSector::dfSector(std::ifstream& infile)
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
		std::vector <std::string> tokens;

		std::stringstream check1(line);
		while (std::getline(check1, dump, ' '))
		{
			if (dump.length() > 0) {
				tokens.push_back(dump);
			}
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
				m_originalFloor = m_floorAltitude = -std::stof(tokens[2]);
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
				m_originalceiling = m_ceilingAltitude = -std::stof(tokens[2]);
			}
		}
		else if (tokens[0] == "SECOND") {
			if (tokens[1] == "ALTITUDE") {
				//PASS
			}
			else if (tokens[1] == "ALTITUDE") {
				//PASS
			}
		}
		else if (tokens[0] == "VERTICES") {
			nbVertices = std::stoi(tokens[1]);
			m_vertices.resize(nbVertices);
			m_wallsLink.resize(nbVertices);
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
}

/**
 * Add a trigger to the sector. Create a boundingbox to detect the trigger
 */
void dfSector::addTrigger(dfLogicTrigger* trigger)
{
	unsigned int wallID = trigger->wall();

	if (wallID < m_walls.size()) {
		// create a bounding box for the trigger. Based off the wall
		dfWall* wall = m_walls[wallID];

		trigger->boundingBox(
			m_vertices[wall->m_left], m_vertices[wall->m_right],
			m_floorAltitude, m_ceilingAltitude
			);

		m_triggers.push_back(trigger);
	}
}

/**
 * Check all triggers to find if one collide with the source box
 */
void dfSector::testTriggers(fwAABBox& box)
{
	// convert from opengl space to level space
	fwAABBox mybox( box.m_x * 10, box.m_x1 * 10, box.m_z * 10, box.m_z1 * 10, box.m_y * 10, box.m_y1 * 10);

	for (auto trigger : m_triggers) {
		if (trigger->collide(mybox)) {
			trigger->activate();
		}
	}
}

/**
 * check if point is inside the boundingbox and inside the 2D surface : external polylines
 * TODO : how to deal with holes ?
 */
bool dfSector::isPointInside(glm::vec3 &p)
{
	// quick check against the 3D bounding box
	if (!m_boundingBox.inside(p)) {
		return false;
	}

	// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
	// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	bool inside = false;
	std::vector<Point>& outline = m_polygons[0];

	/*
	TODO: these solution is supposed to deal with holes, but actually it doesn't work for secbase, when entering sector 58
	for (unsigned int i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++)
	{
		if ((m_vertices[i].y > p.y) != (m_vertices[j].y > p.y) &&
			p.x < (m_vertices[j].x - m_vertices[i].x) * (p.y - m_vertices[i].y) / (m_vertices[j].y - m_vertices[i].y) + m_vertices[i].x)
		{
			inside = !inside;
		}
	}
	*/

	// only test the external line
	inside = false;
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
float dfSector::moveFloor(float delta)
{
	m_floorAltitude += delta;
	m_ceilingAltitude += delta;

	std::cout << m_floorAltitude << " " << m_ceilingAltitude << std::endl;

	if (m_super) {
		m_super->updateSectorVertices(m_id);	// update the sector

		// update the connected portals
		for (auto portal : m_portals) {
			m_super->updateSectorVertices(portal);
		}
	}

	return m_floorAltitude;
}

/**
 * parse all vertices of the sector to link the walls together
 */
std::vector<std::vector<Point>>& dfSector::linkWalls(void)
{
	int links = 0;
	for (unsigned int start = 0; start < m_wallsLink.size(); start++) {
		if (!m_wallsLink[start].parsed) {
			std::vector<Point> polygon;

			// vertice was not yet checked, so start a link
			unsigned int i;

			for (i = start; 
					m_wallsLink[i].m_right != start &&		// detect loop
					m_wallsLink[i].m_right != -1 &&			// detect open lines
					!m_wallsLink[i].parsed;					// detect point used multiple times
				i = m_wallsLink[i].m_right					// move to next vertice
				) 
			{
				m_wallsLink[i].parsed = true;
				Point p = { m_vertices[i].x, m_vertices[i].y };
				polygon.push_back(p);
			}
			Point p = { m_vertices[i].x, m_vertices[i].y };
			polygon.push_back(p);
			m_wallsLink[i].parsed = true;

			// only create polygons for closed line and at least 3 vertices
			if (polygon.size() >= 3 && m_wallsLink[i].m_right != -1) {
				m_polygons.push_back(polygon);
			}
		}
	}

	// find what is the perimeter and what is the hole
	// TODO deal with more than 1 hole
	if (m_polygons.size() > 1) {

		std::vector<Point>& polygon = m_polygons[1];
		Point p = m_polygons[0][1];

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
			std::vector<Point> swap = m_polygons[0];
			m_polygons[0] = m_polygons[1];
			m_polygons[1] = swap;
		}
	}

	return m_polygons;
}

dfSector::~dfSector()
{
	for (auto wall: m_walls) {
		delete wall;
	}
}