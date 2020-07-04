#include "dfLevel.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include "../config.h"
#include "../include/stb_image.h"

#include "../framework/geometries/fwPlaneGeometry.h"
#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaBoundingBoxes.h"
#include "../gaEngine/gaCollisionPoint.h"

#include "dfLogicElevator.h"
#include "dfBitmap.h"
#include "dfSign.h"
#include "dfMesh.h"
#include "dfParserObjects.h"
#include "dfFileSystem.h"
#include "dfAtlasTexture.h"
#include "dfPalette.h"

dfLevel::dfLevel(dfFileSystem* fs, std::string file)
{
	int size;
	char* sec = fs->load(DF_DARK_GOB, file+".LEV", size);
	std::istringstream data(sec);

	std::string line, dump;
	std::map<std::string, std::string> tokenMap;

	while (std::getline(data, line))
	{
		// ignore comment
		if (line[0] == '#' || line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);

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
			// beware of sectors with the same same !
			if (m_sectorsName.count(sector->m_name) == 0) {
				m_sectorsName[sector->m_name] = sector;
			}
			else {
				m_sectorsName[sector->m_name + "(1)"] = sector;
			}
			m_sectorsID[nSector] = sector;

			sector->linkWalls();	// build the polygons from the sector

			// record the sky
			if (sector->m_flag1 & dfSectorFlag::EXTERIOR_NO_CEIL) {
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

	// For every subSector, find the sector it is included in
	for (auto i = 0; i < m_sectorsID.size(); i++) {
		if (m_sectorsID[i]->flag(dfSectorFlag::SUBSECTOR)) {
			for (auto j = 0; j < m_sectorsID.size(); j++) {
				// ignore self
				if (i == j) {
					continue;
				}

				// test and link if needed
				m_sectorsID[i]->includedIn(m_sectorsID[j]);
			}
		}
	}

	// load and ditribute the INF file
	m_inf = new dfParseINF(fs, file);

	// load the Object file
	m_objects = new dfParserObjects(fs, m_palette, file, this);
	m_sprites = m_objects->buildAtlasTexture();
	m_sprites->save("D:/dev/Project1/Project1/images/sprites.png");

	// bind the sectors to the elevator logic
	// bind the evelator logic to the level
	for (auto elevator : m_inf->m_elevators) {
		m_elevators[elevator->name()] = elevator;
		elevator->parent(this);

		dfSector* sector = m_sectorsName[elevator->sector()];
		if (sector) {
			elevator->bindSector(sector);
		}
	}

	// bind the trigger to the elevator
	for (auto trigger : m_inf->m_triggers) {
		dfSector* sector = m_sectorsName[trigger->sector()];
		dfLogicElevator* elevator = m_elevators[sector->m_name];

		trigger->elevator(elevator);
	}

	// bind the sector walls to the triggers
	// build the bounding box of the trigger from the wall of the sector
	std::string sectorname;
	bool sectorIsElevator;
	for (auto trigger : m_inf->m_triggers) {
		sectorname = trigger->sector();
		dfSector* sector = m_sectorsName[sectorname];
		trigger->addEvents(sector);

		if (sector) {
			sectorIsElevator = false;

			// if one of the client of trigger is the very sector the trigger is attached to
			std::vector<dfMessage>& messages = trigger->messages();
			for (auto& message : messages) {
				std::string& client = message.client();
				if (client == sectorname) {
					sectorIsElevator = true;
					break;
				}
			}

			// then the sector is actually an elevator, and the bounding box of the trigger has to be connected to the bounding box of the elevator
			if (sectorIsElevator) {
				trigger->boundingBox(m_elevators[sectorname]);
			}
			else {
				// the trigger is attached to a static wall
				sector->setTriggerFromWall(trigger);
			}


			sector->addTrigger(trigger);
		}
	}

	// bind the trigger to it's client
	for (auto trigger : m_inf->m_triggers) {
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
	m_atlasTexture->save("D:/dev/Project1/Project1/images/textures.png");
	m_atlasTexture->bindToMaterial(m_material);

	spacePartitioning();		// partion of space for quick move
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

	// finaly create the sky as a skybox
	if (m_skyAltitude > 0) {
		dfBitmapImage* image = m_bitmaps[(int)m_skyTexture.r]->getImage();
		m_skybox = image->convert2skyline();
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
	static std::string door = "door";
	static std::string hold = "hold";
	static std::string switch1 = "switch1";

	for (auto sector: m_sectorsID) {
		if (sector->flag() & dfSectorFlag::DOOR) {
			dfLogicElevator* elevator = new dfLogicElevator(door, sector, this);
			dfLogicStop* closed = new dfLogicStop(elevator, sector->referenceFloor(), hold);
			dfLogicStop* opened = new dfLogicStop(elevator, sector->referenceCeiling(), 5000);

			elevator->addStop(closed);
			elevator->addStop(opened);

			m_inf->m_elevators.push_back(elevator);

			// create a trigger based on the full sector (already registered in the elevator)
			dfLogicTrigger* trigger = new dfLogicTrigger(switch1, elevator);

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
		else if (elevator->is(dfElevatorType::MORPH_SPIN1) ||
				elevator->is(dfElevatorType::MORPH_MOVE1)) {
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

	if (elevator->is(dfElevatorType::MORPH_SPIN1) || 
		elevator->is(dfElevatorType::MORPH_MOVE1) ||
		elevator->is(dfElevatorType::MOVE_CEILING) ||
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
	static dfMessage messages[32];
	static int first = 0;

	std::list<gaEntity*> collisions;

	g_MessageBus.findAABBCollision(player, collisions);

	if (collisions.size() > 0) {
		dfMessage* message;

		for (auto entity : collisions) {
			message = messages + first;

			//TODO get the keys the player owns
			message->m_server = "player";
			message->m_client = entity->name();
			message->m_action = DF_MESSAGE_TRIGGER;

			// ignore the player
			if (message->m_client != "player") {
				g_MessageBus.push(message);

				first++;
				if (first == 32) {
					first = 0;
				}
			}
		}
	}
}

/**
 * return the sector fitting the GL position
 */
dfSector* dfLevel::findSector(glm::vec3& position)
{
	dfSector* sector;
	// std::cout << position.x << ":" << position.y << ":" << position.z << std::endl;

	// position is in opengl space
	// TODO should move the opengl <-> level space conversion on a central place
	glm::vec3 level_space;
	gl2level(position, level_space);

	// quick check on the last sector
	if (m_lastSector) {
		// do a full test including holes
		if (m_lastSector->isPointInside(level_space, true)) {
			return m_lastSector;
		}

		// nope, so quick check on the last super sector
		sector = m_lastSuperSector->findSector(level_space);
		if (sector) {
#ifdef DEBUG
			gaDebugLog(LOW_DEBUG, "dfLevel::findSector", " leave=" + m_lastSector->m_name + " enter=" + sector->m_name);
#endif

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
			if (m_lastSector) {
				m_lastSector->event(DF_ELEVATOR_LEAVE_SECTOR);
			}

#ifdef DEBUG
			std::string message = "dfLevel::findSector leave=";
			if (m_lastSector) {
				message += m_lastSector->m_name;
			}
			gaDebugLog(LOW_DEBUG, "dfLevel::findSector", message);

			message = " enter=" + sector->m_name;
			gaDebugLog(LOW_DEBUG, "dfLevel::findSector", message);
#endif

			m_lastSector = sector;
			m_lastSuperSector = ssector;

			sector->event(DF_ELEVATOR_ENTER_SECTOR);
			return sector;
		}
	}

	return nullptr;	// not here
}

/**
 * Parse all sectors to find the one with the position
 * provided by level space
 */
dfSector* dfLevel::findSectorLVL(glm::vec3& level_position)
{
	for (auto sector : m_sectorsID) {
		if (sector->inAABBox(level_position)) {
			return sector;
		}
	}
	return nullptr;
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
	if (m_skybox != nullptr && !m_skyboxAdded) {
		scene->background((fwBackground*)m_skybox);
		m_skyboxAdded = true;
	}

	// add the sprites
	m_objects->add2scene(scene);

	// draw the boundingboxes
	g_gaBoundingBoxes.draw(scene);
}

/**
 * Check move against any of the level component, static Wall and dynamic
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
		// mesh move is done on glSpace
		if (elevator->checkCollision(step, position, target, radius, collision)) {
			return true;
		}
	}
	return false;
}

/**
 * Check move against any of the level component, static (wall, floor, ceiling) and dynamic
 */
bool dfLevel::checkEnvironement(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions)
{
	// us the center of the cylinder to search for the sector
	glm::vec3 p = bounding.position();	
	p.y += bounding.height() / 2.0f;

	dfSector* currentSector = findSector(p);
	if (!currentSector) {
		return false;
	}

	// convert glSpace to dfSpace
	fwCylinder dfCurrent;
	glm::vec3 dfIntersection;
	gl2level(bounding, dfCurrent);

	glm::vec3 dfDirection;
	gl2level(direction, dfDirection);

	// check against static walls, floor and ceiling
	if (currentSector->checkCollision(dfCurrent, dfDirection, dfIntersection, collisions)) {
		for (auto& collision : collisions) {
			// convert back to glSpace
			level2gl(collision.m_position);

			if (collision.m_location == fwCollisionLocation::FRONT) {
				// if we are hitting a wall there is no need to check the elevators
				return true;
			}
		}
	}

	// check against the elevators
	for (auto elevator : m_inf->m_elevators) {
		// mesh move is done on glSpace
		if (elevator->checkCollision(bounding, direction, intersection, collisions)) {
			break;
		}
	}

	// check against the objects of the level
	m_objects->checkCollision(bounding, direction, intersection, collisions);

	return collisions.size() != 0;
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

void dfLevel::level2gl(glm::vec3& v)
{
	float x = v.x / 10.0f,
		y = v.z / 10.0f,
		z = v.y / 10.0f;

	v.x = x;
	v.y = y;
	v.z = z;
}

void dfLevel::level2gl(fwCylinder& level, fwCylinder& gl)
{
	gl.copy(level);
	glm::vec3 p;
	gl2level(level.position(), p);
	gl.position(p);
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

/**
 * Convert gl space to level space
 */
void dfLevel::gl2level(fwCylinder& gl, fwCylinder& level)
{
	level.copy(gl, 10.0f);
	glm::vec3 p;
	gl2level(gl.position(), p);
	level.position(p);
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