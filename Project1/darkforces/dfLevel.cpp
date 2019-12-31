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

			dfSector* sector = new dfSector(infile);
			sector->m_id = nSector;
			int layer = sector->m_layer;

			// record the sector in the global list
			m_sectors[nSector] = sector;
		}
	}
	infile.close();

	compressTextures();	// load textures in a megatexture
	convert2geometry();	// convert sectors to a geometry and apply the mega texture
	spacePartitioning();	// partion of space for quick collision
}

/***
 * Load a texture and store in the list of texture
 * TODO deal with animated textures : ZASWIT*
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

/***
 * store all textures in a megatexture
 * basic algorithm : use a square placement map, find an empty spot, store the texture. if no spot can be found, increase the texture size
 */
void dfLevel::compressTextures(void)
{
	std::list<dfTexture*> sorted_textures;

	// count number of 16x16 blocks
	int blocks16x16=0;
	int bx, by;

	for (auto texture : m_textures) {
		bx = texture->width / 16;
		by = texture->height / 16;
		texture->bsize = bx * by;

		blocks16x16 += texture->bsize;

		sorted_textures.push_back(texture);
	}

	// sort textures by size : big first
	sorted_textures.sort([](dfTexture* a, dfTexture* b) { return a->bsize > b->bsize; });

	// evaluate the size of the megatexture (square texture of 16x16 blocks)
	int bsize = ceil(sqrt(blocks16x16));

	// try to place all objects on the map.
	// if fail, increase the size of the map until we place all textures

	bool allplaced = false;
	bool* placement_map;
	int size;

	do {
		// avaibility map of 16x16 blocks : false = available, true = used
		placement_map = new bool[bsize * bsize]();

		// megatexture in pixel (1 block = 16 pixel)
		size = bsize * 16;
		m_megatexture = new unsigned char[size * size * 3];

		// parse textures and place them on the megatexture
		// find an available space on the map
		int px, py;
		int c = 0;

		allplaced = true;	// suppose we will be able to fit all textures

		for (auto texture : sorted_textures) {
			bx = texture->width / 16;
			by = texture->height / 16;

			// find a spot of the placement map
			px = py = 0;
			bool ok = false;

			for (auto i = 0; i < bsize * bsize; i++) {
				// test each position to find one we can start checking
				if (!placement_map[py * bsize + px]) {
					ok = true;	// suppose we have a spot

					// check availability on the Y axis
					for (auto y = py; y < py + by; y++) {
						// check availability on the X axis
						for (auto x = px; x < px + bx; x++) {
							if (placement_map[y * bsize + x]) {
								ok = false;	// actually, no we don't have a spot
								break;
							}
						}
					}
				}

				// found a spot
				if (ok) {
					break;
				}

				// progress along the placement map. 
				px++;
				if (px >= bsize - bx) {
					px = 0;
					py++;
					if (py >= bsize - by) {
						break;
					}
				}
			}

			// was not able to find a spot.
			if (!ok) {
				// need to increase the size of the map
				allplaced = false;
				bsize++;
				delete placement_map;
				delete m_megatexture;

				break;
			}
			c++;

			// mark the object on the placement map
			for (auto y = py; y < py + by; y++) {
				for (auto x = px; x < px + bx; x++) {
					placement_map[y * bsize + x] = true;
				}
			}

			// copy the texture into the map
			int source_line = 0;
			int bytes = texture->width * 3;	// number of bytes per line
			int dest_line = py * 16 * size * 3 + px * 16 * 3;

			for (auto y = 0; y < texture->height; y++) {
				// copy one line
				memcpy(m_megatexture + dest_line, texture->data + source_line, bytes);
				source_line += bytes;
				dest_line += size * 3;
			}

			// TODO point to the megatexture (use small epsilon to avoid texture bleeding)
			// https://gamedev.stackexchange.com/questions/46963/how-to-avoid-texture-bleeding-in-a-texture-atlas
			texture->m_xoffset = (((px + bx) * 16.0)) / size;
			texture->m_yoffset = (((py + by) * 16.0)) / size;

			texture->m_mega_width = - bx * 16.0 / size;
			texture->m_mega_height = - by * 16.0 / size;
		}
	} while (!allplaced);

	// delete old textures data
	for (auto texture : sorted_textures) {
		free(texture->data);
		texture->data = nullptr;
	}

	// create the fwTexture
	m_fwtextures = new fwTexture(m_megatexture, size, size, 3);

	// and the index
	m_megatexture_idx.resize(m_textures.size());

	int i = 0;
	for (auto texture : m_textures) {
		m_megatexture_idx[i++] = glm::vec4(texture->m_xoffset, texture->m_yoffset, texture->m_mega_width, texture->m_mega_height);
	}
	m_shader_idx = new fwUniform("megatexture_idx", &m_megatexture_idx[0], i);
}

/***
 * create vertices for a rectangle
 */
void dfLevel::addRectangle(dfSector *sector, dfWall *wall, float z, float z1, int texture)
{
	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	int textureID = wall->m_tex[texture].x;

	dfTexture* dfTexture = m_textures[textureID];

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = dfTexture->width;
	float ypixel = dfTexture->height;

	// convert height and length into local texture coordinates using pixel ratio
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks
	float height = abs(z1 - z) * 8.0 / ypixel;
	float width = length * 8.0 / xpixel;

	// get local texture offset on the wall
	// TODO: current supposion : offset x 1 => 1 pixel from the begining on XXX width pixel texture
	float xoffset = (wall->m_tex[texture].y * 8) / xpixel;
	float yoffset = (wall->m_tex[texture].z * 8) / ypixel;

	// resize the opengl buffers
	int p = m_vertices.size();
	m_vertices.resize(p + 6);
	m_uvs.resize(p + 6);
	m_textureID.resize(p + 6);

	// first triangle
	m_vertices[p].x = x / 10;
	m_vertices[p].z = y / 10;
	m_vertices[p].y = z / 10;
	m_uvs[p] = glm::vec2(xoffset, yoffset);
	m_textureID[p] = textureID;

	m_vertices[p + 1].x = x1 / 10;
	m_vertices[p + 1].z = y1 / 10;
	m_vertices[p + 1].y = z / 10;
	m_uvs[p + 1] = glm::vec2(width+xoffset, yoffset);
	m_textureID[p + 1] = textureID;

	m_vertices[p + 2].x = x1 / 10;
	m_vertices[p + 2].z = y1 / 10;
	m_vertices[p + 2].y = z1 / 10;
	m_uvs[p + 2] = glm::vec2(width+xoffset, height+yoffset);
	m_textureID[p + 2] = textureID;

	// second triangle
	m_vertices[p + 3].x = x / 10;
	m_vertices[p + 3].z = y / 10;
	m_vertices[p + 3].y = z / 10;
	m_uvs[p + 3] = glm::vec2(xoffset, yoffset);
	m_textureID[p + 3] = textureID;

	m_vertices[p + 4].x = x1 / 10;
	m_vertices[p + 4].z = y1 / 10;
	m_vertices[p + 4].y = z1 / 10;
	m_uvs[p + 4] = glm::vec2(width+xoffset, height+yoffset);
	m_textureID[p + 4] = textureID;

	m_vertices[p + 5].x = x / 10;
	m_vertices[p + 5].z = y / 10;
	m_vertices[p + 5].y = z1 / 10;
	m_uvs[p + 5] = glm::vec2(xoffset, height+yoffset);
	m_textureID[p + 5] = textureID;
}

/**
 * Convert a level into a megga textured mesh
 */
void dfLevel::convert2geometry(void)
{
	int size = 0;
	int p = 0;

	for (auto sector : m_sectors) {
		if (sector->m_layer != 1) {
			continue;
		}
		for (auto wall : sector->m_walls) {
			if (wall->m_adjoint < 0) {
				// full wall
				addRectangle(sector, wall,
					sector->m_floorAltitude,
					sector->m_ceilingAltitude,
					DFWALL_TEXTURE_MID
				);
			}
			else {
				// portal
				dfSector *portal = m_sectors[wall->m_adjoint];

				if (portal->m_ceilingAltitude < sector->m_ceilingAltitude) {
					// add a wall above the portal
					addRectangle(sector, wall, 
						portal->m_ceilingAltitude,
						sector->m_ceilingAltitude,
						DFWALL_TEXTURE_TOP
					);
				}
				if (portal->m_floorAltitude > sector->m_floorAltitude) {
					// add a wall below the portal
					addRectangle(sector, wall,
						sector->m_floorAltitude,
						portal->m_floorAltitude,
						DFWALL_TEXTURE_BOTTOM
					);
				}
			}
		}
	}

	size = m_vertices.size();
	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", &m_vertices[0], 3, size * sizeof(glm::vec3), sizeof(float), false);
	m_geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, &m_uvs[0], 2, size * sizeof(glm::vec2), sizeof(float), false);
	m_geometry->addAttribute("aTextureID", GL_ARRAY_BUFFER, &m_textureID[0], 1, size * sizeof(float), sizeof(float), false);
}

/**
 * partition of sectors by layers for faster search
 */
void dfLevel::spacePartitioning(void)
{
	// get number of layers
	int layer = 0, max_layers = 0;
	for (auto sector : m_sectors) {
		layer = sector->m_layer;
		if (layer > max_layers) {
			m_layers.resize(layer + 1);
			max_layers++;
		}

		m_layers[layer].m_sectors.push_back(sector);
		m_layers[layer].m_boundingBox.extend(sector->m_boundingBox);
	}
}

/**
 * return the sector fitting the position
 */
dfSector* dfLevel::findSector(glm::vec3& position)
{
	// position is in opengl space
	glm::vec3 level_space = position;
	level_space *= 10;

	for (auto layer : m_layers) {
		if (layer.m_boundingBox.inside(level_space)) {
			for (auto sector : layer.m_sectors) {
				if (sector->isPointInside(level_space)) {
					return sector;
				}
			}
		}
	}

	return nullptr;	// not here
}

dfLevel::~dfLevel()
{
	for (auto sector : m_sectors) {
		delete sector;
	}
	for (auto texture : m_textures) {
		delete texture;
	}
	//delete m_megatexture;
	delete m_fwtextures;
}