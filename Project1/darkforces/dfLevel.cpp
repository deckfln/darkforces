#include "dfLevel.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#include "../config.h"
#include "../framework/geometries/fwPlaneGeometry.h"
#include "../include/stb_image.h"

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
			if (dump.length() > 0) {
				tokens.push_back(dump);
			}
		}

		if (tokens[0] == "LEV") {
			m_level = tokens[1];
		}
		else if (tokens[0] == "LEVELNAME") {
			m_name = tokens[1];
		}
		else if (tokens[0] == "TEXTURES") {
			int nbTextures = std::stoi(tokens[1]);
			m_textures.resize(nbTextures);
		}
		else if (tokens[0] == "TEXTURE:") {
			std::string bm = tokens[1];
			loadGeometry(bm);
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

	compressTextures();	// load textures in a megatexture
	convert2geometry();	// convert sectors to a geometry and apply the mega texture
}

/***
 * Load a texture and store in the list of texture
 */
void dfLevel::loadGeometry(std::string file)
{
	int index = file.find(".BM");
	file.replace(index, 3, ".png");
	file = "data/textures/" + file;

	dfTexture* texture = new dfTexture;
	texture->data = stbi_load(file.c_str(), &texture->width, &texture->height, &texture->nrChannels, STBI_rgb);
	texture->m_name = file;
	m_textures[m_currentTexture++] = texture;
}


static float quadUvs[] = { 0.0f, 1.0f,	0.0f, 0.0f,	1.0f, 0.0f,	0.0f, 1.0f,	1.0f, 0.0f,	1.0f, 1.0f };

/***
 * compress all textures of the same size in megatexture
 */
void dfLevel::compressTextures(void)
{
	// count number of textures by size
	std::list<dfTexture*> textures[4];

	for (auto texture : m_textures) {
		if (texture->width != 64) {
			continue;
		}
		switch (texture->height) {
		case 64:
			textures[0].push_back(texture); break;
		case 128:
			textures[1].push_back(texture); break;
		case 256:
			textures[2].push_back(texture); break;
		case 512:
			textures[3].push_back(texture); break;
		default:
			printf("texture height > 512\n");
		}
	}

	for (auto i = 0; i < 1; i++) {
		// TODO : size for non-square texture
		// 4 => 2x2
		// 5 => 3x3
		int size = ceil(sqrt(textures[i].size()));
		int source_line = 0;
		int dest_line = 0;

		// TODO : memory for non-square texture
		unsigned char* data = m_megatexture[i] = new unsigned char[size * size * 64 * 64 * 3];
		
		for (auto y = 0; y < size; y++) {
			for (auto x = 0; x < size; x++) {
				if (textures[i].size() == 0) {
					break;
				}

				source_line = 0;
				// TODO : position for non-square texture
				dest_line = y * size * 64 * 64 * 3 + x * 64 * 3;

				dfTexture* texture = textures[i].front();
				for (auto p = 0; p < texture->height; p++) {
					// copy one line of 64 bytes
					memcpy(data + dest_line, texture->data + source_line, 64 * 3);
					source_line += 64 * 3;
					dest_line += size * 64 * 3;
				}
				// delete old source data
				free(texture->data);
				texture->data = nullptr;

				// point to the megatexture
				// TODO: y-offset for non-square texture
				texture->m_texture = i;
				texture->m_xoffset = x / size;
				texture->m_yoffset = y / size;

				textures[i].pop_front();
			}
		}

		// create the fwTexture
		m_fwtextures[i] = new fwTexture(m_megatexture[i], pow(2, i + 6) * size, pow(2, i+6) * size, 3);
	}
}

/***
 * create vertices for a rectangle
 */
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
	for (auto texture : m_textures) {
		delete texture;
	}
	for (auto i = 0; i < 4; i++) {
		delete m_megatexture[i];
	}
}