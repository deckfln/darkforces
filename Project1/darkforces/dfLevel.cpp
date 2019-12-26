#include "dfLevel.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "../config.h"
#include "../framework/geometries/fwPlaneGeometry.h"

dfLevel::dfLevel(std::string file)
{
	std::ifstream infile(ROOT_FOLDER + "/" + file);
	std::string line, dump;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line[0] == '#' || line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens;

		std::stringstream check1(line);
		while (std::getline(check1, dump, ' '))
		{
			tokens.push_back(dump);
		}

		if (tokens[0] == "LEV") {
			m_level = tokens[1];
		}
		else if (tokens[0] == "LEVELNAME") {
			m_name = tokens[1];
		}
		else if (tokens[0] == "TEXTURES") {
			int nbTextures = std::stoi(tokens[1]);
		}
		else if (tokens[0] == "NUMSECTORS") {
			int nbSectors = std::stoi(tokens[1]);
			m_sectors.resize(nbSectors);
		}
		else if (tokens[0] == "SECTOR") {
			int nSector = std::stoi(tokens[1]);
			m_sectors[nSector] = new dfSector(infile);
		}
	}
	infile.close();

	convert2geometry();
}

static float quadUvs[] = { 0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 1.0f,	1.0f, 0.0f,	1.0f, 1.0f };

static void addRectangle(std::vector <glm::vec3> &vertices, float x, float y, float z, float x1, float y1, float z1)
{
	int p = vertices.size();
	vertices.resize(p + 6);

	// first triangle
	vertices[p].x = x / 10;
	vertices[p].z = y / 10;
	vertices[p].y = z / 10;

	vertices[p + 1].x = x1 / 10;
	vertices[p + 1].z = y1 / 10;
	vertices[p + 1].y = z / 10;

	vertices[p + 2].x = x1 / 10;
	vertices[p + 2].z = y1 / 10;
	vertices[p + 2].y = z1 / 10;

	// second triangle
	vertices[p + 3].x = x / 10;
	vertices[p + 3].z = y / 10;
	vertices[p + 3].y = z / 10;

	vertices[p + 4].x = x1 / 10;
	vertices[p + 4].z = y1 / 10;
	vertices[p + 4].y = z1 / 10;

	vertices[p + 5].x = x / 10;
	vertices[p + 5].z = y / 10;
	vertices[p + 5].y = z1 / 10;
}

void dfLevel::convert2geometry(void)
{
	std::vector <glm::vec3> vertices;

	int size = 0;
	int p = 0;

	for (auto sector : m_sectors) {
		if (sector->m_layer != 1) {
			continue;
		}
		for (auto wall : sector->m_walls) {
			// ignore portals
			if (wall->m_adjoint < 0) {
				// full wall
				addRectangle(vertices,
					sector->m_vertices[wall->m_left].x, sector->m_vertices[wall->m_left].y, sector->m_floorAltitude,
					sector->m_vertices[wall->m_right].x, sector->m_vertices[wall->m_right].y, sector->m_ceilingAltitude
				);
			}
			else {
				// portal
				dfSector *portal = m_sectors[wall->m_adjoint];

				if (portal->m_ceilingAltitude < sector->m_ceilingAltitude) {
					// add a wall above the portal
					addRectangle(vertices,
						sector->m_vertices[wall->m_left].x, sector->m_vertices[wall->m_left].y, portal->m_ceilingAltitude,
						sector->m_vertices[wall->m_right].x, sector->m_vertices[wall->m_right].y, sector->m_ceilingAltitude
					);
				}
				if (portal->m_floorAltitude > sector->m_floorAltitude) {
					// add a wall below the portal
					addRectangle(vertices,
						sector->m_vertices[wall->m_left].x, sector->m_vertices[wall->m_left].y, sector->m_floorAltitude,
						sector->m_vertices[wall->m_right].x, sector->m_vertices[wall->m_right].y, portal->m_floorAltitude
					);
				}
			}
		}
	}

	m_vertices = new glm::vec3[vertices.size()];
	memcpy(m_vertices, &vertices[0], vertices.size() * sizeof(glm::vec3));

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", m_vertices, 3, vertices.size() * sizeof(glm::vec3), sizeof(float), false);
	m_geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
}

dfLevel::~dfLevel()
{
	delete[] m_vertices;

	for (auto sector : m_sectors) {
		delete sector;
	}
}