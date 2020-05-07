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
#include "dfParserObjects.h"
#include "dfFileSystem.h"
#include "dfAtlasTexture.h"

dfLevel::dfLevel(dfFileSystem* fs, std::string file)
{
	char* sec = fs->load(DF_DARK_GOB, file+".LEV");
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
			m_palette = new dfPalette(fs, tokens[1]);
		}
		else if (tokens[0] == "TEXTURES") {
			int nbTextures = std::stoi(tokens[1]);
			m_bitmaps.resize(nbTextures);
		}
		else if (tokens[0] == "TEXTURE:") {
			std::string bm = tokens[1];
			loadBitmaps(fs, bm);
		}
		else if (tokens[0] == "NUMSECTORS") {
			int nbSectors = std::stoi(tokens[1]);
			m_sectorsID.resize(nbSectors);
		}
		else if (tokens[0] == "SECTOR") {
			int nSector = std::stoi(tokens[1]);

			dfSector* sector = new dfSector(data, m_sectorsID);
			sector->m_id = nSector;
			if (sector->m_name == "") {
				sector->m_name = std::to_string(nSector);
			}
			int layer = sector->m_layer;

			// record the sector in the global list
			m_sectorsName[sector->m_name] = sector;
			m_sectorsID[nSector] = sector;

			sector->linkWalls();	// build the polygons from the sector

			// record the sky
			if (sector->m_flag1 & DF_SECTOR_EXTERIOR_NO_CEIL) {
				m_skyAltitude = std::max(m_skyAltitude, sector->ceiling() + 100);
				m_skyTexture = sector->m_ceilingTexture;
			}

			// keep track of the bounding box
			m_boundingBox.extend(sector->m_boundingBox);
		}
	}

	// bind the sectors to adjoint sectors and to mirror walls
	for (auto sector : m_sectorsID) {
		sector->bindWall2Sector();
	}

	// load and ditribute the INF file
	m_inf = new dfParseINF(fs, file);

	// load the Object file
	m_objects = new dfParserObjects(fs, m_palette, file);
	m_sprites = m_objects->buildAtlasTexture();
	m_sprites->save("/dev/Project1/Project1/images/atlas.png");

	// bind the sectors to the elevator logic
	// bind the evelator logic to the level
	for (auto elevator : m_inf->m_elevators) {
		elevator->parent(this);

		dfSector* sector = m_sectorsName[elevator->sector()];
		if (sector) {
			elevator->bindSector(sector);
		}
	}

	// bind the sector walls to the elevator logic
	for (auto trigger : m_inf->m_triggers) {
		dfSector* sector = m_sectorsName[trigger->sector()];
		trigger->addEvents(sector);

		if (sector) {
			sector->setTriggerFromWall(trigger);
			sector->addTrigger(trigger);
		}

		std::vector<dfMessage>& messages = trigger->messages();
		for (auto& message : messages) {
			std::string& client = message.client();
			dfSector* sector = m_sectorsName[client];
			sector->addTrigger(trigger);
		}
	}

	convertDoors2Elevators();	// for every sector 'DOOR', create an evelator and a trigger

	m_material = new fwMaterialBasic("data/shaders/vertex.glsl", "", "data/shaders/fragment.glsl");

	// load textures in a megatexture
	m_atlasTexture = new dfAtlasTexture(m_allTextures);
	m_atlasTexture->bindToMaterial(m_material);
	m_atlasTexture->save("/dev/Project1/Project1/images/atlas.png");

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
void dfLevel::loadBitmaps(dfFileSystem *fs, std::string file)
{
	dfBitmap *bitmap = new dfBitmap(fs, file, m_palette);
	dfBitmapImage* image;

	m_bitmaps[m_currentBitmap++] = bitmap;	// LEV based bitmaps (related to floors and walls)

	// extract all versions of the same bitmap, so many more textures
	for (int i = bitmap->nbImages() - 1; i >= 0; i--) {
		image = bitmap->getImage(i);
		m_allTextures.push_back(image);
	}
}

/**
 * create geometries for all super sectors
 */
void dfLevel::buildGeometry(void)
{
	for (auto ssector : m_supersectors) {
		ssector->buildGeometry(m_sectorsID);
	}

	// finaly create the sky ceiling
	if (m_skyAltitude > 0) {
		float width = std::max(m_boundingBox.m_p1.x - m_boundingBox.m_p.x, m_boundingBox.m_p1.y - m_boundingBox.m_p.y);

		dfBitmapImage* image = m_bitmaps[(int)m_skyTexture.r]->getImage();
		m_sky = new dfMesh(m_material, m_bitmaps);
		m_sky->addPlane(width * 3, image);
		fwMesh *mesh = m_sky->buildMesh();
		glm::vec3 center = glm::vec3((m_boundingBox.m_p1.x + m_boundingBox.m_p.x) / 20.0f,
			m_skyAltitude / 10.0f,	
			(m_boundingBox.m_p1.y + m_boundingBox.m_p.y) / 20.0f);
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
	vssectors.resize(m_sectorsID.size());
	int i = 0;
	for (auto sector : m_sectorsID) {
		dfSuperSector *ssector = new dfSuperSector(sector, m_material, m_bitmaps);
		m_supersectors.push_back(ssector);
		vssectors[i++] = ssector;
	}

	// create the portals
	for (auto &ssector : m_supersectors) {
		ssector->buildPortals(m_sectorsID, vssectors);
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
		ssector->buildHiearchy(this);
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
		elevator->buildGeometry(m_material, m_bitmaps);
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

	for (auto sector: m_sectorsID) {
		if (sector->flag() & DF_SECTOR_DOOR) {
			dfLogicElevator* elevator = new dfLogicElevator(inv, sector, this);
			dfLogicStop* closed = new dfLogicStop(elevator, sector->referenceFloor(), hold);
			dfLogicStop* opened = new dfLogicStop(elevator, sector->referenceCeiling(), 5000);

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
		else if (elevator->is(DF_ELEVATOR_MORPH_SPIN1) ||
				elevator->is(DF_ELEVATOR_MORPH_MOVE1)) {
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
	for (auto trigger : m_inf->m_triggers) {
		if (trigger->collide(player)) {
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

/**
 * Convert level space to gl space
 */
void dfLevel::level2gl(glm::vec3& level, glm::vec3& gl)
{
	gl.x = level.x / 10.0f;
	gl.y = level.z / 10.0f;
	gl.z = level.y / 10.0f;
}

/**
 * Convert gl space to level space
 */
void dfLevel::gl2level(glm::vec3& gl, glm::vec3& level)
{
	level.x = gl.x * 10.0f;
	level.y = gl.z * 10.0f;
	level.z = gl.y * 10.0f;
}

dfLevel::~dfLevel()
{
	for (auto sector : m_sectorsID) {
		delete sector;
	}
	for (auto bitmap : m_bitmaps) {
		delete bitmap;
	}
	delete m_inf;
	delete m_objects;
	delete m_palette;
	delete m_atlasTexture;
	delete m_material;
}