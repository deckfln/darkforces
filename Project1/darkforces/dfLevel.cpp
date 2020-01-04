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
			loadBitmaps(bm);
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

	buildAtlasMap();	// load textures in a megatexture
	spacePartitioning();// partion of space for quick collision
	buildGeometry();	// build the geometry of each super sectors
}

/***
 * Load a texture and store in the list of texture
 * TODO deal with animated textures : ZASWIT*
 */
void dfLevel::loadBitmaps(std::string file)
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
 * store all textures in an atlas map
 * basic algorithm : use a square placement map, find an empty spot, store the texture. if no spot can be found, increase the texture size
 */
void dfLevel::buildAtlasMap(void)
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
	int bsize = (int)ceil(sqrt(blocks16x16));

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
				delete[] placement_map;
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
			texture->m_xoffset = (((px + bx) * (float)16.0)) / size;
			texture->m_yoffset = (((py + by) * (float)16.0)) / size;

			texture->m_mega_width = - bx * (float)16.0 / size;
			texture->m_mega_height = - by * (float)16.0 / size;
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

/**
 * create geometries for all super sectors
 */
void dfLevel::buildGeometry(void)
{
	m_material = new fwMaterialBasic("data/shaders/vertex.glsl", "", "data/shaders/fragment.glsl");
	m_material->addDiffuseMap(m_fwtextures);
	m_material->addUniform(m_shader_idx);

	for (auto ssector : m_supersectors) {
		ssector->buildGeometry(m_sectors, m_textures, m_material);
	}
}

/**
 * partition of sectors into superSector
 */
void dfLevel::spacePartitioning(void)
{
	// convert sectors into super sectors
	// build both a list to quickly sort and shrink and a vector to convert sectorID to superSectorID
	std::vector<dfSuperSector*> vssectors;
	vssectors.resize(m_sectors.size());
	int i = 0;
	for (auto sector : m_sectors) {
		dfSuperSector *ssector = new dfSuperSector(sector);
		m_supersectors.push_back(ssector);
		vssectors[i++] = ssector;
	}

	// create the portals
	for (auto &ssector : m_supersectors) {
		ssector->buildPortals(m_sectors, vssectors);
	}

	// target 64 supersector
	// TODO : define as a constant or a value based on the number of sectors
	while (m_supersectors.size() > 64) {
		m_supersectors.sort([](dfSuperSector* a, dfSuperSector* b) { return a->boundingBoxSurface() > b->boundingBoxSurface(); });

		// pick the smallest sector and merge it to its smallest adjoint
		dfSuperSector* smallest = m_supersectors.back();
		dfSuperSector* smallest_adjoint = smallest->smallestAdjoint();
		if (smallest_adjoint) {
			smallest_adjoint->extend(smallest);
		}
		else {
			// no adjoint left or empty sector
		}

		// TODO will need to be deleted
		//delete smallest;
		m_supersectors.pop_back();
	}
}

/**
 * return the SuperSector fitting the position
 */
dfSuperSector* dfLevel::findSuperSector(glm::vec3& position)
{
	// position is in opengl space
	// TODO should move the opengl <-> level space conversion on a central place
	glm::vec3 level_space = position;
	level_space *= 10;

	// std::cout << position.x << ":" << position.y << ":" << position.z << std::endl;
	for (auto ssector : m_supersectors) {
		if (ssector->inAABBox(level_space)) {
			return ssector;
		}
	}

	return nullptr;	// not here
}

/**
 * return the sector fitting the position
 */
dfSector* dfLevel::findSector(glm::vec3& position)
{
	dfSector* sector;
	// std::cout << position.x << ":" << position.y << ":" << position.z << std::endl;

	// position is in opengl space
	// TODO should move the opengl <-> level space conversion on a central place
	glm::vec3 level_space = position;
	level_space *= 10;
	float z = level_space.z;
	level_space.z = level_space.y;
	level_space.y = z;

	// quick check on the last sector
	if (m_lastSector) {
		if (m_lastSector->isPointInside(level_space)) {
			return m_lastSector;
		}

		// nope, so quick check on the last super sector
		sector = m_lastSuperSector->findSector(level_space);
		if (sector) {
			m_lastSector = sector;
			return sector;
		}
	}

	// still nope, full search
	for (auto ssector: m_supersectors) {
		sector = ssector->findSector(level_space);

		if (sector) {
			m_lastSector = sector;
			m_lastSuperSector = ssector;
			return sector;
		}
	}

	return nullptr;	// not here
}

/**
 * parse the super sectors to find which one are in the camera frustrum
 * use the portals to drill through portals
 */
void dfLevel::draw(fwCamera* camera, fwScene* scene)
{
	glm::vec3 position = camera->get_position();

	//TODO move the position to the feet of the player, but on the controler
	position.y -= 0.3;

	// mark all supersectors as NO VISBILE
	for (auto ssector : m_supersectors) {
		ssector->visible(false);
	}

	// use the cached values to find the current super sector
	dfSector *sector = findSector(position);
	if (sector) {
		m_lastSuperSector->visible(true);

		// recusivly test the portals to make supersectors visible in the camera frustrum 
		m_lastSuperSector->checkPortals(camera);
	}
	// ELSE outside of the map

	// parse the scene to update the supersectors visibility
	for (auto ssector : m_supersectors) {
		ssector->add2scene(scene);
	}
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
