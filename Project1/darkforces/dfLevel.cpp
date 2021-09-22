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

#include "../gaEngine/World.h"
#include "../gaEngine/gaBoundingBoxes.h"
#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/gaMessage.h"
#include "../gaEngine/World.h"
#include "../gaEngine/gaNavMesh.h"

#include "dfBitmap.h"
#include "dfMesh.h"
#include "dfParserObjects.h"
#include "dfFileSystem.h"
#include "dfAtlasTexture.h"
#include "dfPalette.h"
#include "dfLogicStop.h"
#include "dfParseINF.h"

#include "dfComponent/InfElevator.h"
#include "dfComponent/Trigger.h"

#include "gaEntity/ElevatorDoor.h"

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

			dfSector* sector = new dfSector(data, m_sectorsID, this);
			sector->m_id = nSector;
			if (sector->name() == "") {
				sector->name(std::to_string(nSector));
			}
			int layer = sector->m_layer;

			// record the sector in the global list
			// beware of sectors with the same !
			if (m_sectorsName.count(sector->name()) == 0) {
				m_sectorsName[sector->name()] = sector;
			}
			else {
				m_sectorsName[sector->name() + "(1)"] = sector;
			}
			m_sectorsID[nSector] = sector;

			sector->linkWalls();	// build the polygons from the sector

			// record the sky
			if (sector->m_flag1 & dfSectorFlag::EXTERIOR_NO_CEIL) {
				m_skyAltitude = std::max(m_skyAltitude, sector->ceiling() + 100);
				m_skyTexture = sector->m_ceilingTexture;
			}

			// keep track of the bounding box
			m_boundingBox.extend(sector->m_worldAABB);
		}
	}

	// bind the sectors to adjoint sectors and to mirror walls
	for (auto sector : m_sectorsID) {
		sector->bindWall2Sector();
	}

	// For every subSector, find the sector it is included in
	for (uint32_t i = 0; i < m_sectorsID.size(); i++) {
		if (m_sectorsID[i]->flag(dfSectorFlag::SUBSECTOR)) {
			for (uint32_t j = 0; j < m_sectorsID.size(); j++) {
				// ignore self
				if (i == j) {
					continue;
				}

				// test and link if needed
				m_sectorsID[i]->includedIn(m_sectorsID[j]);
			}
		}
	}

	// initialize opengl Stuff
	m_material = new fwMaterialBasic("data/shaders/vertex.glsl", "", "data/shaders/fragment.glsl");

	// load textures in a megatexture
	m_atlasTexture = new dfAtlasTexture(m_allTextures);
	m_atlasTexture->save("D:/dev/Project1/Project1/images/textures.png");
	m_atlasTexture->bindToMaterial(m_material);

	// load and distribute the INF file
	m_inf = new dfParseINF(fs, file, this);

	// find all sectors with a InfProgram component and add them to the world
	for (auto sector : m_sectorsID) {
		if (sector->components() > 0) {
			g_gaWorld.addClient(sector);
		}
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
			std::vector<gaMessage*>& messages = trigger->messages();
			for (auto& message : messages) {
				const std::string& client = message->client();
				if (client == sectorname) {
					sectorIsElevator = true;
					break;
				}
			}

			// then the sector is actually an elevator, and the bounding box of the trigger has to be connected to the bounding box of the elevator
			if (!sectorIsElevator) {
				// the trigger is attached to a static wall
				sector->setTriggerFromWall(trigger);
			}

			//sector->addTrigger(trigger);

			for (auto& message : messages) {
				const std::string& client = message->client();
				dfSector* sector = m_sectorsName[client];

				// find the elevator bound to the sector
				DarkForces::Component::InfElevator* elevator = dynamic_cast<DarkForces::Component::InfElevator*>(sector->findComponent(DF_COMPONENT_INF_ELEVATOR));

				// find the trigger bound to the dfLogicTriger
				DarkForces::Component::Trigger* cTrigger = dynamic_cast<DarkForces::Component::Trigger*>(trigger->findComponent(DF_COMPONENT_TRIGGER));

				// and bind the elevator and the trigger
				elevator->addTrigger(cTrigger);
			}
		}

		g_gaWorld.addClient(trigger);
	}

	spacePartitioning();		// partition of space for quick move
	buildGeometry();			// build the geometry of each super sectors

	convertDoors2Elevators();	// for every sector 'DOOR', create an elevator and a trigger

	// Add doors
	for (auto door : m_doors) {
		g_gaWorld.addClient(door);
	}

	// Add the missing triggers to the world
	for (auto trigger : m_inf->m_triggers) {
		if (g_gaWorld.getEntity(trigger->name()) == nullptr) {
			g_gaWorld.addClient(trigger);
		}
	}

	// Add SuperSectors
	fwScene* scene = static_cast<fwScene*>(g_gaWorld.get("scene"));
	for (auto ssector : m_supersectors) {
		g_gaWorld.addClient(ssector);

		ssector->rebuildScene(scene);
	}

	// load the Object file
	m_objects = new dfParserObjects(fs, m_palette, file, this);
	m_sprites = m_objects->buildAtlasTexture();
	m_sprites->save("D:/dev/Project1/Project1/images/sprites.png");

	// start with all supersectors hidden
	hideSectors();

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
	std::vector<dfSuperSector*> to_remove;

	for (auto ssector : m_supersectors) {
		if (ssector->buildGeometry(m_sectorsID) == nullptr) {
			// some supersector are replaced by elevators, so they have no geometry
			to_remove.push_back(ssector);
		}
	}

	for (auto ssector : to_remove) {
		m_supersectors.remove(ssector);
	}

	// finally create the sky as a sky box
	if (m_skyAltitude > 0) {
		dfBitmapImage* image = m_bitmaps[(int)m_skyTexture.r]->getImage();
		m_skybox = image->convert2skyline();
	}

	// and last of the last build the navMesh
	g_navMesh.buildMesh();
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

	// target 64 super sector
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

		//delete smallest;
		smallest->remove();
		m_supersectors.pop_back();
	}

	for (auto ssector : m_supersectors) {
		// sort the sectors in each super sector per size
		ssector->sortSectors();

		// create a full hierarchy
		ssector->buildHiearchy(this);

		// record the super sector in the world
		g_gaWorld.addSector(ssector);
	}

	// delete unneeded super sector
	for (auto ssector : vssectors) {
		if (ssector->removed()) {
			delete ssector;
		}
	}

}

/**
 * for every sector 'DOOR', create an elevator and it stops PLUS trigger
 */
void dfLevel::convertDoors2Elevators(void)
{
	for (auto sector: m_sectorsID) {
		if (sector->flag() & dfSectorFlag::DOOR) {
			DarkForces::Entity::ElevatorDoor* door = new DarkForces::Entity::ElevatorDoor(sector);
			//m_inf->m_triggers.push_back(door->trigger());
			m_doors.push_back(door);
		}
	}
}

/**
 * return the sector fitting the GL position
 */
dfSector* dfLevel::findSector(const glm::vec3& position)
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
		if (m_lastSector->isPointInside(position, true)) {
			return m_lastSector;
		}

		// nope, so quick check on the last super sector
		sector = m_lastSuperSector->findDFSector(position);
		if (sector) {
#ifdef _DEBUG
			gaDebugLog(LOW_DEBUG, "dfLevel::findSector", " leave=" + m_lastSector->name() + " enter=" + sector->name());
#endif

			m_lastSector = sector;
			return sector;
		}
	}

	// still nope, full search
	for (auto ssector: m_supersectors) {
		sector = ssector->findDFSector(position);

		if (sector) {
			if (m_lastSector) {
				m_lastSector->event(DarkForces::MessageEvent::LEAVE_SECTOR);
			}

#ifdef _DEBUG
			std::string message = "dfLevel::findSector leave=";
			if (m_lastSector) {
				message += m_lastSector->name();
			}
			gaDebugLog(LOW_DEBUG, "dfLevel::findSector", message);

			message = " enter=" + sector->name();
			gaDebugLog(LOW_DEBUG, "dfLevel::findSector", message);

#endif

			m_lastSector = sector;
			m_lastSuperSector = ssector;

			return sector;
		}
	}

	// force on the last position
	return m_lastSector;
}

/**
 * find sector by name
 */
dfSector* dfLevel::findSector(char* name)
{
	return m_sectorsName[name];
}

dfSector* dfLevel::findSector(const std::string& name)
{
	return m_sectorsName[name];
}

/**
 * Parse all sectors to find the one with the position
 * provided by level space
 */
dfSector* dfLevel::findSectorLVL(const glm::vec3& level_position)
{
	glm::vec3 gl_position;
	dfLevel::level2gl(level_position, gl_position);
	for (auto sector : m_sectorsID) {
		if (sector->inAABBox(gl_position)) {
			return sector;
		}
	}
	return nullptr;
}

/**
 * register the skymap on the scene
 */
void dfLevel::addSkymap(fwScene* scene)
{
	scene->background((fwBackground*)m_skybox);
}

/**
 * Convert level space to opengl space
 */
void dfLevel::level2gl(const glm::vec3& level, glm::vec3& gl)
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
 * Convert opengl space to level space
 */
void dfLevel::gl2level(const glm::vec3& gl, glm::vec3& level)
{
	level.x = gl.x * 10.0f;
	level.y = gl.z * 10.0f;
	level.z = gl.y * 10.0f;
}

/**
 * Convert opengl space to level space
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
	for (auto door : m_doors) {
		delete door;
	}

	delete m_inf;
	delete m_objects;
	delete m_palette;
	delete m_atlasTexture;
	delete m_material;
}