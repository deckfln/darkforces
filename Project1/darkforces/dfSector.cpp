#include "dfSector.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

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
			else if (tokens[1] == "ALTITUDE") {
				//PASS
			}
		}
		else if (tokens[0] == "VERTICES") {
			nbVertices = std::stoi(tokens[1]);
			m_vertices.resize(nbVertices);
		}
		else if (tokens[0] == "X:") {
			float x  = -std::stof(tokens[1]);
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

			m_walls[currentWall++] = wall;
		}
	}

	m_boundingBox = fwAABBox(min_x, max_x, min_y, max_y, m_floorAltitude, m_ceilingAltitude);
}

/**
 * check if point is inside the boundingbox and inside the 2D surface
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
	for (unsigned int i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++)
	{
		if ((m_vertices[i].y > p.y) != (m_vertices[j].y > p.y) &&
			p.x < (m_vertices[j].x - m_vertices[i].x) * (p.y - m_vertices[i].y) / (m_vertices[j].y - m_vertices[i].y) + m_vertices[i].x)
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

dfSector::~dfSector()
{
	for (auto wall: m_walls) {
		delete wall;
	}
}