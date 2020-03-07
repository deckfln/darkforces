#include "dfLevel.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include "../config.h"
#include "../framework/geometries/fwPlaneGeometry.h"
#include "../include/stb_image.h"
#include "dfLogicElevator.h"
#include "dfBitmap.h"
#include "dfSign.h"
#include "dfMesh.h"

dfLevel::dfLevel(dfFileGOB* dark, dfFileGOB* gTextures, std::string file)
{
	char* sec = dark->load(file+".LEV");
	std::istringstream data(sec);

	std::string line, dump;

	while (std::getline(data, line))
	{
		// ignore comment
		if (line[0] == '#' || line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line);

		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "LEV") {
			m_level = tokens[1];
		}
		else if (tokens[0] == "LEVELNAME") {
			m_name = tokens[1];
		}
		else if (tokens[0] == "PALETTE") {
			m_palette = new dfPalette(dark, tokens[1]);
		}
		else if (tokens[0] == "TEXTURES") {
			int nbTextures = std::stoi(tokens[1]);
			m_bitmaps.resize(nbTextures);
		}
		else if (tokens[0] == "TEXTURE:") {
			std::string bm = tokens[1];
			loadBitmaps(gTextures, bm);
		}
		else if (tokens[0] == "NUMSECTORS") {
			int nbSectors = std::stoi(tokens[1]);
			m_sectors.resize(nbSectors);
		}
		else if (tokens[0] == "SECTOR") {
			int nSector = std::stoi(tokens[1]);

			dfSector* sector = new dfSector(data);
			sector->m_id = nSector;
			if (sector->m_name == "") {
				sector->m_name = std::to_string(nSector);
			}
			int layer = sector->m_layer;

			// record the sector in the global list
			m_sectors[nSector] = sector;
			m_hashSectors[sector->m_name] = sector;

			sector->linkWalls();	// build the polygons from the sector

			// record the sky
			if (sector->m_flag1 & DF_SECTOR_EXTERIOR_NO_CEIL) {
				m_skyAltitude = std::max(m_skyAltitude, sector->m_ceilingAltitude + 100);
				m_skyTexture = sector->m_ceilingTexture;
			}

			// keep track of the bounding box
			m_boundingBox.extend(sector->m_boundingBox);
		}
	}

	// bind the sectors to adjoint sectors and to mirror walls
	for (auto sector : m_sectors) {
		sector->bindWall2Sector(m_sectors);
	}

	// load and ditribute the INF file
	m_inf = new dfParseINF(dark, file);

	// bind the sectors to the elevator logic
	// bind the evelator logic to the level
	for (auto elevator : m_inf->m_elevators) {
		elevator->parent(this);

		dfSector* sector = m_hashSectors[elevator->sector()];
		if (sector) {
			elevator->bindSector(sector);
		}
	}

	// bind the sector walls to the elevator logic
	for (auto trigger : m_inf->m_triggers) {
		dfSector* sector = m_hashSectors[trigger->sector()];
		trigger->addEvents(sector);

		if (sector) {
			sector->setTriggerFromWall(trigger);
		}
	}

	convertDoors2Elevators();	// for every sector 'DOOR', create an evelator and a trigger
	buildAtlasMap();			// load textures in a megatexture

	m_material = new fwMaterialBasic("data/shaders/vertex.glsl", "", "data/shaders/fragment.glsl");
	m_material->addDiffuseMap(m_fwtextures);
	m_material->addUniform(m_shader_idx);

	spacePartitioning();		// partion of space for quick collision
	buildGeometry();			// build the geometry of each super sectors

	createTriggers();		// for elevator_spin1, create triggers
	initElevators();			// move all elevators to position 0

	free(sec);
}

/***
 * Load a texture and store in the list of texture
 * TODO deal with animated textures : ZASWIT*
 */
void dfLevel::loadBitmaps(dfFileGOB *gob, std::string file)
{
	dfBitmap *bitmap = new dfBitmap(gob, file, m_palette);
	dfBitmapImage* image;

	m_bitmaps[m_currentBitmap++] = bitmap;	// LEV based bitmaps (related to floors and walls)

	// extract all versions of the same bitmap, so many more textures
	for (int i = bitmap->nbImages() - 1; i >= 0; i--) {
		image = bitmap->getImage(i);
		m_allTextures.push_back(image);
	}
}

/***
 * store all textures in an atlas map
 * basic algorithm : use a square placement map, find an empty spot, store the texture. if no spot can be found, increase the texture size
 */
void dfLevel::buildAtlasMap(void)
{
	std::list<dfBitmapImage*> sorted_textures;
	const int blockSize = 4;

	// count number of 16x16 blocks
	int blocks16x16=0;
	int bx, by;

	for (auto texture : m_allTextures) {
		if (texture) {
			bx = texture->m_width / blockSize;
			by = texture->m_height / blockSize;
			texture->bsize = bx * by;

			blocks16x16 += texture->bsize;

			sorted_textures.push_back(texture);
		}
	}

	// sort textures by size : big first
	sorted_textures.sort([](dfBitmapImage* a, dfBitmapImage* b) { return a->bsize > b->bsize; });

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
		size = bsize * blockSize;
		m_megatexture = new unsigned char[size * size * 3];

		// parse textures and place them on the megatexture
		// find an available space on the map
		int px, py;
		int c = 0;

		allplaced = true;	// suppose we will be able to fit all textures

		for (auto texture : sorted_textures) {
			bx = texture->m_width / blockSize;
			by = texture->m_height / blockSize;

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
			int bytes = texture->m_width * 3;	// number of bytes per line
			int dest_line = py * blockSize * size * 3 + px * blockSize * 3;

			for (auto y = 0; y < texture->m_height; y++) {
				// copy one line
				memcpy(m_megatexture + dest_line, texture->m_data + source_line, bytes);
				source_line += bytes;
				dest_line += size * 3;
			}

			// TODO point to the megatexture (use small epsilon to avoid texture bleeding)
			// https://gamedev.stackexchange.com/questions/46963/how-to-avoid-texture-bleeding-in-a-texture-atlas
			texture->m_xoffset = (((px + bx) * (float)blockSize)) / size;
			texture->m_yoffset = (((py + by) * (float)blockSize)) / size;

			texture->m_mega_width = - bx * (float)blockSize / size;
			texture->m_mega_height = - by * (float)blockSize / size;
		}
	} while (!allplaced);

	// delete old textures data
//	for (auto texture : sorted_textures) {
//		free(texture->m_data);
//		texture->m_data = nullptr;
//	}

	// create the fwTexture
	m_fwtextures = new fwTexture(m_megatexture, size, size, 3);

	// and the index
	m_megatexture_idx.resize(m_allTextures.size());

	int i = 0;
	for (auto texture : m_allTextures) {
		if (texture) {
			m_megatexture_idx[i] = glm::vec4(texture->m_xoffset, texture->m_yoffset, texture->m_mega_width, texture->m_mega_height);
			texture->m_textureID = i;
			i++;
		}
	}
	m_shader_idx = new fwUniform("megatexture_idx", &m_megatexture_idx[0], i);
}

/**
 * create geometries for all super sectors
 */
void dfLevel::buildGeometry(void)
{
	for (auto ssector : m_supersectors) {
		ssector->buildGeometry(m_sectors, m_material);
	}

	// finaly create the sky ceiling
	if (m_skyAltitude > 0) {
		float width = std::max(m_boundingBox.m_x1 - m_boundingBox.m_x, m_boundingBox.m_y1 - m_boundingBox.m_y);

		dfBitmapImage* image = m_bitmaps[(int)m_skyTexture.r]->getImage();
		m_sky = new dfMesh(m_material);
		m_sky->addPlane(width, image);
		fwMesh *mesh = m_sky->buildMesh();
		glm::vec3 center = glm::vec3((m_boundingBox.m_x1 + m_boundingBox.m_x) / 20.0f,
			m_skyAltitude / 10.0f,	
			(m_boundingBox.m_y1 + m_boundingBox.m_y) / 20.0f);
		mesh->translate(center);
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

	for (auto ssector : m_supersectors) {
		// sort the sectors in each supersector per size
		ssector->sortSectors();

		// create a full hierarchy
		ssector->parent(this);
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
 * init all elevators of the level to their HOLD position
 */
void dfLevel::initElevators(void)
{
	for (auto elevator : m_inf->m_elevators) {
		// build a mesh and store the mesh in the super-sector holding the sector
		elevator->buildGeometry(m_material);
		elevator->init(0);
	}
}

/**
 * for every sector 'DOOR', create an evelator and it stops PLUS trigger
 */
void dfLevel::convertDoors2Elevators(void)
{
	static std::string inv = "inv";
	static std::string hold = "hold";
	static std::string switch1 = "switch1";

	for (auto sector : m_sectors) {
		if (sector->flag() & DF_SECTOR_DOOR) {
			dfLogicElevator* elevator = new dfLogicElevator(inv, sector, this);
			dfLogicStop* closed = new dfLogicStop(elevator, sector->m_floorAltitude, hold);
			dfLogicStop* opened = new dfLogicStop(elevator, sector->m_ceilingAltitude, 5000);

			elevator->addStop(closed);
			elevator->addStop(opened);

			m_inf->m_elevators.push_back(elevator);

			dfLogicTrigger* trigger = new dfLogicTrigger(switch1, sector, 0, elevator);

			// once the elevator closes, send a DONE message to the trigger
			dfMessage msg(DF_MESSAGE_DONE, 0, trigger->name());
			closed->addMessage(msg);

			trigger->config();
			m_inf->m_triggers.push_back(trigger);
		}
	}
}

/**
 * for every elevator that has NO explicit switch, create a trigger
 */
void dfLevel::createTriggers(void)
{
	std::map <std::string, bool> explicits;

	// get of all elevators with an explicit switch
	for (auto trigger : m_inf->m_triggers) {
		for (auto& target : trigger->clients()) {
			explicits[target] = true;
		}
	}

	// for all elevators that send messages to other elevator => explicit
	for (auto elevator : m_inf->m_elevators) {
		std::list<std::string> sectors;
		elevator->getMessagesToSectors(sectors);
		for (auto& target : sectors) {
			explicits[target] = true;
		}
	}

	// create a trigger for any elevator that is not on the explicit list
	for (auto elevator : m_inf->m_elevators) {
		if (explicits.count(elevator->name()) == 0) {
			createTriggerForElevator(elevator);
		}
	}
}

/**
 * Create dedicated trigger by elevator class
 */
void dfLevel::createTriggerForElevator(dfLogicElevator *elevator)
{
	static std::string standard = "switch1";

	if (elevator->is(DF_ELEVATOR_MORPH_SPIN1) || 
		elevator->is(DF_ELEVATOR_MORPH_MOVE1) ||
		elevator->is(DF_ELEVATOR_MOVE_CEILING) ||
		elevator->needsKeys()
		) {
		dfLogicTrigger* trigger = new dfLogicTrigger(standard, elevator);
		trigger->config();

		// extract the 'CLOSED' stop = (0)
		// add a message DONE on the stop
		dfMessage msg(DF_MESSAGE_DONE, 0, trigger->name());
		elevator->stop(0)->addMessage(msg);

		m_inf->m_triggers.push_back(trigger);
	}
}

/**
 * Check all triggers to find if one collide with the source box
 */
void dfLevel::testSwitch(fwAABBox& player)
{
	// convert from opengl space to level space
	fwAABBox mybox(player.m_x * 10, player.m_x1 * 10, player.m_z * 10, player.m_z1 * 10, player.m_y * 10, player.m_y1 * 10);

	for (auto trigger : m_inf->m_triggers) {
		if (trigger->collide(mybox)) {
			//TODO get the keys the player owns
			trigger->activate(DF_KEY_RED);
		}
	}
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
	glm::vec3 level_space(
		position.x * 10.0f,
		position.z * 10.0f,
		position.y * 10.0f);

	// quick check on the last sector
	if (m_lastSector) {
		// do a full test including holes
		if (m_lastSector->isPointInside(level_space, true)) {
			return m_lastSector;
		}

		// nope, so quick check on the last super sector
		sector = m_lastSuperSector->findSector(level_space);
		if (sector) {
			std::cerr << "dfLevel::findSector leave=" << m_lastSector->m_name << " enter=" << sector->m_name << std::endl;

			m_lastSector->event(DF_ELEVATOR_LEAVE_SECTOR);

			m_lastSector = sector;

			sector->event(DF_ELEVATOR_ENTER_SECTOR);
			return sector;
		}
	}

	// still nope, full search
	for (auto ssector: m_supersectors) {
		sector = ssector->findSector(level_space);

		if (sector) {
			std::cerr << "dfLevel::findSector leave=";
			if (m_lastSector) {
				std::cerr << m_lastSector->m_name;
				m_lastSector->event(DF_ELEVATOR_LEAVE_SECTOR);
			}
			std::cerr << " enter=" << sector->m_name << std::endl;

			m_lastSector = sector;
			m_lastSuperSector = ssector;

			sector->event(DF_ELEVATOR_ENTER_SECTOR);
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
	position.y -= 0.3f;

	// mark all supersectors as NO VISBILE
	for (auto ssector : m_supersectors) {
		ssector->visible(false);
	}

	// use the cached values to find the current super sector
	dfSector *sector = findSector(position);
	if (sector) {
		m_lastSuperSector->visible(true);

		// recusivly test the portals to make supersectors visible in the camera frustrum 
		m_lastSuperSector->checkPortals(camera, 1);
	}
	// ELSE outside of the map

	// parse the scene to update the supersectors visibility
	for (auto ssector : m_supersectors) {
		ssector->add2scene(scene);
	}

	// add the sky
	if (m_skymesh == nullptr) {
		m_skymesh = m_sky->mesh();
		scene->addChild(m_skymesh);
	}
}

/**
 * Check collision against any of the level component, static and dynamic
 */
bool dfLevel::checkCollision(float step, glm::vec3& position, glm::vec3& target, float height, float radius, glm::vec3& collision)
{
	dfSector* currentSector = findSector(position);
	if (!currentSector) {
		return false;
	}

	// convert glSpace to dfSpace
	glm::vec3 dfCurrent(position.x * 10.0f, position.z * 10.0f, position.y * 10.0f);
	glm::vec3 dfTarget(target.x * 10.0f, target.z * 10.0f, target.y * 10.0f);
	glm::vec3 dfNew;

	// check against static walls
	if (currentSector->checkCollision(step*10.f, dfCurrent, dfTarget, height*10.0f, radius*10.f, dfNew)) {
		// convert back to glSpace
		collision.x = dfNew.x / 10.0f;
		collision.y = dfNew.z / 10.0f;
		collision.z = dfNew.y / 10.0f;
		return true;
	}

	// check against the elevators
	for (auto elevator : m_inf->m_elevators) {
		// mesh collision is done on glSpace
		if (elevator->checkCollision(step, position, target, radius, collision)) {
			return true;
		}
	}
	return false;
}

dfLevel::~dfLevel()
{
	for (auto sector : m_sectors) {
		delete sector;
	}
	for (auto bitmap : m_bitmaps) {
		delete bitmap;
	}
	delete m_palette;
	delete m_megatexture;
	delete m_fwtextures;
	delete m_inf;
	delete m_shader_idx;
	delete m_material;
}