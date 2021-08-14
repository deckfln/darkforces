#include "dfSector.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#include <algorithm>
#include <glm/glm.hpp>

#include "../config.h"

#include "../framework/math/fwCylinder.h"
#include "../framework/fwCollision.h"

#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/World.h"

#include "dfSuperSector.h"
#include "dfMesh.h"
#include "dfParseINF.h"
#include "dfSign.h"
#include "dfLevel.h"

dfSector::dfSector(std::istringstream& infile, std::vector<dfSector*>& sectorsID):
	gaEntity(DF_ENTITY_SECTOR),
	m_sectorsID(sectorsID)
{
	physical(false);	// object is a virtual entity (cannot collide)

	int nbVertices;
	int currentVertice = 0;

	int nbWalls;
	int currentWall = 0;

	std::string line, dump;
	std::map<std::string, std::string> tokenMap;

	float min_x=99999, max_x=-99999, min_y=99999, max_y=-99999;	// build AABBox while parsing the vertices's

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
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "NAME" && tokens.size() > 1) {
			m_name = tokens[1];
		}
		if (tokens[0] == "LAYER") {
			m_layer = std::stoi(tokens[1]);
		}
		else if (tokens[0] == "AMBIENT") {
			m_currentAmbient = m_ambient = std::stof(tokens[1]);
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
				m_staticMeshFloorAltitude = m_referenceFloorAltitude = m_floorAltitude = -std::stof(tokens[2]);
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
				m_staticMeshCeilingAltitude = m_referenceCeilingAltitude = m_ceilingAltitude = -std::stof(tokens[2]);
			}
		}
		else if (tokens[0] == "SECOND") {
			if (tokens[1] == "ALTITUDE") {
				//PASS
			}
		}
		else if (tokens[0] == "FLAGS") {
			m_flag1 = std::stoi(tokens[1]);
			m_flag2 = std::stoi(tokens[2]);
			m_flag3 = std::stoi(tokens[3]);
		}
		else if (tokens[0] == "VERTICES") {
			nbVertices = std::stoi(tokens[1]);
			m_vertices.resize(nbVertices);
			m_verticeConnexions.resize(nbVertices);
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
			dfWallFlag flag1 = (dfWallFlag)std::stoi(tokens[31]);
			dfWallFlag flag3 = (dfWallFlag)std::stoi(tokens[33]);

			dfWall* wall = new dfWall(left, right, adjoint, mirror, flag1, flag3);
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

	m_height = m_referenceCeilingAltitude - m_referenceFloorAltitude;

	// convert from level space to GL space
	glm::vec3 p(min_x, min_y, m_referenceFloorAltitude);
	glm::vec3 p1(max_x, max_y, m_referenceCeilingAltitude);
	dfLevel::level2gl(p);
	dfLevel::level2gl(p1);
	m_worldAABB = GameEngine::AABBoxTree(p, p1);
}

/**
 * configure the trigger based on the wall of the sector
 */
void dfSector::setTriggerFromWall(dfLogicTrigger* trigger)
{
	unsigned int wallID = trigger->wall();

	if (wallID < m_walls.size()) {
		// create a bounding box for the trigger. Based off the wall
		dfWall* wall = m_walls[wallID];

		trigger->boundingBox(
			m_vertices[wall->m_left], m_vertices[wall->m_right],
			m_referenceFloorAltitude, m_referenceCeilingAltitude
			);
	}
}

/**
 * configure the trigger based on the wall of the sector
 */
void dfSector::setTriggerFromSector(dfLogicTrigger* trigger)
{
	trigger->boundingBox(m_worldAABB);
}

/**
 * configure the trigger based on the floor of the sector
 */
void dfSector::setTriggerFromFloor(dfLogicTrigger* trigger)
{
	glm::vec2 left(m_worldAABB.m_p.x, m_worldAABB.m_p.y);
	glm::vec2 right(m_worldAABB.m_p1.x, m_worldAABB.m_p1.y);

	trigger->boundingBox(
		left, right,
		m_referenceFloorAltitude, m_referenceFloorAltitude
	);

}

/**
 * Move the floor of the sector
 * Also move all triggers on the sector
 */
void dfSector::currentFloorAltitude(float z)
{
	m_floorAltitude = z;
}

/**
 * If the target Z is below the original floor, lower the floor
 */
void dfSector::staticFloorAltitude(float z)
{
	if (z < m_staticMeshFloorAltitude) {
		m_staticMeshFloorAltitude = z;
	}
}

/**
* Return the static floor from that sector, or the parent sector that one is included in
*/
float dfSector::staticFloorAltitude(void)
{
	return m_staticMeshFloorAltitude;
}

/**
 * If the target Z is above the original ceiling
 */
void dfSector::staticCeilingAltitude(float z)
{
	if (z > m_staticMeshCeilingAltitude) {
		m_staticMeshCeilingAltitude = z;
	}
}

/**
 * Return the static ceiling from that sector, or the parent sector that one is included in
 */
float dfSector::staticCeilingAltitude(void)
{
	return m_staticMeshCeilingAltitude;
}

/**
 * Move the ceiling of the sector
 * Also move all triggers on the sector
 */
void dfSector::ceiling(float z)
{
	m_ceilingAltitude = z;
}

/**
 * Return a list of walls based on the sector status
 *	list of ALL walls
 *	list of the external walls
 */
std::vector<dfWall*>& dfSector::walls(dfWallFlag flags)
{
	static std::vector<dfWall*> ml;

	ml.clear();

	switch (flags) {
	case dfWallFlag::ALL:
		return m_walls;
		break;
	case dfWallFlag::MORPHS_WITH_ELEV:
		// only walls that move when the elevator moves
		for (auto wall : m_walls) {
			if (wall->flag1(dfWallFlag::MORPHS_WITH_ELEV)) {
				ml.push_back(wall);
			}
		}
		break;
	case dfWallFlag::NOT_MORPHS_WITH_ELEV:
		// only walls that DO NOT move when the elevator moves
		for (auto wall : m_walls) {
			if (!wall->flag1(dfWallFlag::MORPHS_WITH_ELEV)) {
				ml.push_back(wall);
			}
		}
		break;
	default:
#ifdef _DEBUG
		gaDebugLog(LOW_DEBUG, "dfSector::walls", " flags=" + std::to_string((int)flags) + " not implemented");
#endif
	}

	return ml;
}

/**
 * Return the polygon(s) making the sector, based on the number of polygons to display
 */
std::vector<std::vector<Point>>& dfSector::polygons(int displayPolygon)
{
	static std::vector<std::vector<Point>> ml;

	int dp = (displayPolygon == -1) ? m_displayPolygons : displayPolygon;

	assert(dp >= 0 && dp < 3);

	switch (dp) {
	case 0:
		return m_polygons_vertices;	// all walls
	case 1:
		ml.clear();
		ml.push_back(m_polygons_vertices[0]);
		return ml;	// only the external polygon
	case 2:
		ml.clear();
		ml.push_back(m_polygons_vertices[1]);
		return ml;	// only the hole polygon
	}

	ml.clear();
	return ml;
}

/**
 * Add a mesh to the super sector holding the sector
 */
void dfSector::addObject(dfMesh* object)
{
	m_super->addObject(object);
}

/**
 * check if point is inside the openGL bounding box and inside the 2D surface : external polylines
 */
bool dfSector::isPointInside(const glm::vec3 &p, bool fullTest)
{
	// quick check against the 3D bounding box
	if (!m_worldAABB.inside(p)) {
		return false;
	}

	// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
	// http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
	bool inside = false;

	glm::vec3 level_p;
	dfLevel::gl2level(p, level_p);

	// test holes
	// if asked for AND if the sector default setup is 'handle all' : elevator SPIN1 & MOVE1 ignore hole
	if (fullTest & (m_displayPolygons == 0)) {
		for (unsigned int i = 1; i < m_polygons_vertices.size(); i++) {
			std::vector<Point>& line = m_polygons_vertices[i];

			for (unsigned int i = 0, j = line.size() - 1; i < line.size(); j = i++)
			{
				if ((line[i][1] > level_p.y) != (line[j][1] > level_p.y) &&
					level_p.x < (line[j][0] - line[i][0]) * (level_p.y - line[i][1]) / (line[j][1] - line[i][1]) + line[i][0])
				{
					inside = !inside;
				}

				if (inside) {
					return false;	// if we are in the hole, we are not on the sector
				}
			}

		}
	}

	// finish with the external line
	std::vector<Point>& outline = m_polygons_vertices[0];
	for (unsigned int i = 0, j = outline.size() - 1; i < outline.size(); j = i++)
	{
		if ((outline[i][1] > level_p.y) != (outline[j][1] > level_p.y) &&
			level_p.x < (outline[j][0] - outline[i][0]) * (level_p.y - outline[i][1]) / (outline[j][1] - outline[i][1]) + outline[i][0])
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
	return m_worldAABB.surface();
}

/**
 * parse all vertices of the sector to link the walls together
 */
void dfSector::linkWalls(void)
{
	for (auto wall : m_walls) {
		m_verticeConnexions[wall->m_left].m_rightVertice = wall->m_right;
		m_verticeConnexions[wall->m_right].m_leftVertice = wall->m_left;

		m_verticeConnexions[wall->m_left].m_leftWall = wall->m_id;
		m_verticeConnexions[wall->m_right].m_rightWall = wall->m_id;

		wall->sector(this);
	}

	int links = 0;
	for (unsigned int start = 0; start < m_verticeConnexions.size(); start++) {
		if (!m_verticeConnexions[start].parsed) {
			std::vector<Point> polygon;
			std::vector<dfWall*> polygonWalls;

			// vertice was not yet checked, so start a link
			unsigned int i;

			for (i = start; 
					m_verticeConnexions[i].m_rightVertice != start &&		// detect loop
					m_verticeConnexions[i].m_rightVertice != -1 &&			// detect open lines
					!m_verticeConnexions[i].parsed;					// detect point used multiple times
				i = m_verticeConnexions[i].m_rightVertice					// move to next vertex
				) 
			{
				m_verticeConnexions[i].parsed = true;
				Point p = { m_vertices[i].x, m_vertices[i].y };
				polygon.push_back(p);

				if (m_verticeConnexions[i].m_leftWall >= 0) {
					polygonWalls.push_back(m_walls[m_verticeConnexions[i].m_leftWall]);
				}
			}
			Point p = { m_vertices[i].x, m_vertices[i].y };
			polygon.push_back(p);
			if (m_verticeConnexions[i].m_leftWall >= 0) {
				polygonWalls.push_back(m_walls[m_verticeConnexions[i].m_leftWall]);
			}

			m_verticeConnexions[i].parsed = true;

			// only create polygons for closed line and at least 3 vertices
			if (polygon.size() >= 3 && m_verticeConnexions[i].m_rightVertice != -1) {
				m_polygons_vertices.push_back(polygon);
				m_polygons_walls.push_back(polygonWalls);
			}
		}
	}

	// find what is the perimeter and what is the hole
	// TODO deal with more than 1 hole
	if (m_polygons_vertices.size() > 1) {

		if (m_polygons_vertices.size() > 2) {
#ifdef _DEBUG
			gaDebugLog(LOW_DEBUG, "dfSector::linkWalls", "sector with more than 1 hole is not implemented");
#endif
		}

		// sort the polygons to move the outside polygon at position 0
		for (unsigned int poly_start = 0; poly_start < m_polygons_vertices.size() - 1; poly_start++) {
			Point p = m_polygons_vertices[poly_start][1];

			for (unsigned int poly = poly_start + 1; poly < m_polygons_vertices.size(); poly++) {
				std::vector<Point>& polygon = m_polygons_vertices[poly];

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
					// if polygon N inside polygon N+1
					// move the outside polygon at position N
					std::vector<Point> swap = m_polygons_vertices[poly_start];
					m_polygons_vertices[poly_start] = m_polygons_vertices[poly];
					m_polygons_vertices[poly] = swap;

					std::vector<dfWall*> swap1 = m_polygons_walls[poly_start];
					m_polygons_walls[poly_start] = m_polygons_walls[poly];
					m_polygons_walls[poly] = swap1;
				}
			}
		}
	}
}

/**
 * If all vertices's of the current sectors are also in the target sector
 *   the current sector is included in the target.
 *   register the data and override the sector altitudes
 *   EG: slider_sw is included in 113 on SECBASE.LEV
 */
bool dfSector::includedIn(dfSector* sector)
{
	if (!m_worldAABB.inside(sector->m_worldAABB)) {
		return false;
	}

	bool verticesIn = true;

	for (auto& source : m_vertices) {
		bool verticeIn = false;

		for (auto& target : sector->m_vertices) {
			if (source == target) {
				verticeIn = true;
				break;
			}
		}

		if (!verticeIn) {
			verticesIn = false;
			break;
		}
	}

	if (verticesIn) {
		// register the sectors
		m_includedIn = sector;
		sector->m_includes.push_back(this);
	}

	return verticesIn;
}

/**
 * bind each wall with an adjoint to the other sector
 */
void dfSector::bindWall2Sector(void)
{
	for (auto wall : m_walls) {
		if (wall->m_adjoint >= 0) {
			wall->m_pAdjoint = m_sectorsID[wall->m_adjoint];
			wall->m_pMmirror = wall->m_pAdjoint->m_walls[wall->m_mirror];
		}
	}
}

/**
 * Manage events sent to the sector
 */
void dfSector::event(int event_mask)
{
	if (m_eventMask & event_mask) {
#ifdef _DEBUG
		gaDebugLog(LOW_DEBUG, "dfSector::event", "sector=" + m_name + " event=" + std::to_string(event_mask));
#endif
		g_gaWorld.sendMessage("player", m_name, DF_MESSAGE_TRIGGER, 0, nullptr);
	}
}

/**
 * only keep walls that are on the perimeters
 * hard walls are moved to an elevator mesh
 */
void dfSector::removeHollowWalls(void)
{
	// the list of walls will be the external ring
	//m_walls = m_polygons_walls[0];
	m_displayPolygons = 1;
}

/**
 * // quick test point inside AABB
 */
bool dfSector::inAABBox(const glm::vec3& position)
{
	return m_worldAABB.inside(position);
}

/**
 * quick test to find AABB collision
 */
bool dfSector::collideAABB(const fwAABBox& box)
{
	return m_worldAABB.intersect(box);
}

/**
 * Change the lightning of the sector in the super-sector mesh
 */
void dfSector::changeAmbient(float ambient)
{
	m_currentAmbient = ambient / 32.0f;

	if (m_wallVerticesLen > 0) {
		m_super->updateAmbientLight(m_currentAmbient, m_wallVerticesStart, m_wallVerticesLen);
	}

	if (m_floorVerticesLen > 0) {
		m_super->updateAmbientLight(m_currentAmbient, m_floorVerticesStart, m_floorVerticesLen);
	}
}

/***
 * create a trigger and a geometry for a sign
 */
dfLogicTrigger* dfSector::addSign(dfMesh *mesh, dfWall* wall, float z, float z1, int texture)
{
	// record the sign on the wall
	dfSector* s = wall->sector();
	std::string name = s->m_name + "(" + std::to_string(wall->m_id) + ")";

	dfLogicTrigger* trigger = (dfLogicTrigger*)g_gaWorld.getEntity(name);
	if (trigger) {
		dfSign* sign = new dfSign(mesh, wall->sector(), wall, z, z1);
		trigger->sign(sign);
	}

	return trigger;
}

/**
 * list of signs to add later (likely when the sector is an elevator)
 */
void dfSector::deferedAddSign(dfWall* wall)
{
	m_deferedSigns.push_back(wall);
}

/**
 * Create the signs at the end of the vertice's buffer
 */
void dfSector::buildSigns(dfMesh *mesh)
{
	int size = 0;
	int p = 0;

	for (auto wall : m_walls) {
		if (wall->m_tex[DFWALL_TEXTURE_SIGN].r >= 0) {

			if (wall->m_adjoint < 0) {
				// full wall
				addSign(mesh, wall,
					m_staticMeshFloorAltitude,
					m_staticMeshCeilingAltitude,
					DFWALL_TEXTURE_MID
					);
			}
			else {
				// portal
				dfSector* portal = m_sectorsID[wall->m_adjoint];
				int nbSigns = 0;

				if (portal->m_staticMeshCeilingAltitude < m_staticMeshCeilingAltitude) {
					// add a wall above the portal
					addSign(mesh, wall,
						portal->m_staticMeshCeilingAltitude,
						m_staticMeshCeilingAltitude,
						DFWALL_TEXTURE_TOP
						);
					nbSigns++;
				}
				if (portal->m_staticMeshFloorAltitude > m_staticMeshFloorAltitude) {
					// add a wall below the portal
					addSign(mesh, wall,
						m_staticMeshFloorAltitude,
						portal->m_staticMeshFloorAltitude,
						DFWALL_TEXTURE_BOTTOM
						);
					nbSigns++;
				}

				if (nbSigns == 0) {
					// force a sign on the wall because the portal is not visible
					portal->deferedAddSign(wall);
				}
			}
		}
	}
}

/**
 * Add walls of the sector in the given dfMesh
 */
void dfSector::buildWalls(dfMesh* mesh, dfWallFlag displayPolygon)
{
	std::vector<dfWall*> filtered_walls;

	switch (displayPolygon) {
	case dfWallFlag::ALL:
		filtered_walls = m_walls;
		break;
	case dfWallFlag::MORPHS_WITH_ELEV:
		// only walls that move when the elevator moves
		for (auto wall : m_walls) {
			if (wall->flag1(dfWallFlag::MORPHS_WITH_ELEV)) {
				filtered_walls.push_back(wall);
			}
		}
		break;
	case dfWallFlag::NOT_MORPHS_WITH_ELEV:
		// only walls that DO NOT move when the elevator moves
		for (auto wall : m_walls) {
			if (!wall->flag1(dfWallFlag::MORPHS_WITH_ELEV)) {
				filtered_walls.push_back(wall);
			}
		}
		break;
	default:
#ifdef _DEBUG
		gaDebugLog(LOW_DEBUG, "dfSector::walls", "flags=" + std::to_string((int)displayPolygon) +" not implemented for sector=" + std::to_string(m_id));
#endif
	}

	m_wallVerticesStart = mesh->nbVertices();

	for (auto wall : filtered_walls) {
		if (wall->m_adjoint < 0) {
			// full wall
			mesh->addRectangle(this, wall,
				m_staticMeshFloorAltitude,
				m_staticMeshCeilingAltitude,
				wall->m_tex[DFWALL_TEXTURE_MID],
				m_ambient,
				true
			);
		}
		else {
			// portal
			dfSector* portal = m_sectorsID[wall->m_adjoint];

			if (portal->m_referenceCeilingAltitude < m_referenceCeilingAltitude) {
				// add a wall above the portal
				mesh->addRectangle(this, wall,
					portal->m_staticMeshCeilingAltitude,
					m_staticMeshCeilingAltitude,
					wall->m_tex[DFWALL_TEXTURE_TOP],
					m_ambient,
					true
				);
			}
			if (portal->m_referenceFloorAltitude > m_referenceFloorAltitude) {
				// add a wall below the portal
				mesh->addRectangle(this, wall,
					m_staticMeshFloorAltitude,
					portal->m_staticMeshFloorAltitude,
					wall->m_tex[DFWALL_TEXTURE_BOTTOM],
					m_ambient,
					true
				);
			}
		}
	}

	m_wallVerticesLen = mesh->nbVertices() - m_wallVerticesStart;

}

/**
 * build the floor geometry by triangulating the shape
 * apply texture by using an axis aligned 8x8 grid
 */
void dfSector::buildFloorAndCeiling(dfMesh* mesh)
{
	// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
	// data won't have more than 65536 vertices.
	using N = uint32_t;

	// Ignore strange sector whose ceiling is below the floor
	if (m_staticMeshCeilingAltitude < m_staticMeshFloorAltitude) {
		//TODO do not forget that 'thing'
#ifdef _DEBUG
		gaDebugLog(LOW_DEBUG, "dfSector::buildFloorAndCeiling", "ignore dfSuperSector::buildFloor sector " + std::to_string(m_id));
#endif
		return;
	}

	// Fill polygon structure with actual data. Any winding order works.
	// The first polyline defines the main polygon.
	// Following polylines define holes.
	std::vector<std::vector<Point>>& polygon = polygons(-1);	// default polygons

	m_floorVerticesStart = mesh->nbVertices();
	mesh->addFloor(polygon, m_staticMeshFloorAltitude, m_floorTexture, m_ambient, false);

	// Create the ceiling, unless there is a sky
	if (!(m_flag1 & dfSectorFlag::EXTERIOR_NO_CEIL)) {
		mesh->addFloor(polygon, m_staticMeshCeilingAltitude, m_ceilingTexture, m_ambient, true);
	}
	m_floorVerticesLen = mesh->nbVertices() - m_floorVerticesStart;
}

/**
 * Add the geometry of the sector in the given dfMesh
 */
void dfSector::buildGeometry(dfMesh* mesh, dfWallFlag displayPolygon)
{
	if (m_includedIn != nullptr && m_elevator != nullptr && !m_elevator->is(dfElevator::Type::CHANGE_LIGHT)) {
		// for sectors that are included in other sector as elevator
		// do not add them on the supermesh, they have their own mesh in the elevator
		return;
	}

	m_firstVertex = mesh->nbVertices();

	buildWalls(mesh, displayPolygon);
	buildFloorAndCeiling(mesh);
	buildSigns(mesh);

	m_nbVertices= mesh->nbVertices() - m_firstVertex;

	m_worldAABB.geometry(mesh->vertice(), m_firstVertex, m_nbVertices);
	m_worldAABB.m_extra = &m_name;

	// build hierarchy of AABB
	mesh->addModelAABB(&m_worldAABB);
}

/**
 * Build an outward mesh based on the sector
 */
void dfSector::buildElevator(gaEntity* parent, dfMesh* mesh, float bottom, float top, int what, bool clockwise, dfWallFlag flags)
{
	if (!m_super) {
		return;
	}
	
	std::vector<dfBitmap*>& textures = m_super->textures();

	// create the walls
	std::vector <dfWall*>& wallss = walls(flags);
	for (auto wall : wallss) {
		if (wall->m_adjoint < 0) {
			// full wall (for spin1 elevators)
			mesh->addRectangle(this, wall,
				bottom,
				top,
				wall->m_tex[what],
				m_ambient,
				true
			);
			// PASS
		}
		else {
			// portal
			dfWall* mirror = wall->m_pMmirror;
			dfSector* portal = m_sectorsID[wall->m_adjoint];

			if (what == DFWALL_TEXTURE_MID) {
				// for elevators spin1, spin2 and move1 use the same algorithm than normal walls
				if (portal->m_referenceCeilingAltitude > m_referenceCeilingAltitude) {
					// add a wall above the portal
					mesh->addRectangle(this, wall,
						portal->m_staticMeshCeilingAltitude,
						m_staticMeshCeilingAltitude,
						mirror->m_tex[DFWALL_TEXTURE_TOP],
						m_ambient,
						true
					);
				}
				else if (portal->m_referenceFloorAltitude < m_referenceFloorAltitude) {
					// add a wall below the portal
					mesh->addRectangle(this, wall,
						m_staticMeshFloorAltitude,
						portal->m_staticMeshFloorAltitude,
						mirror->m_tex[DFWALL_TEXTURE_BOTTOM],
						m_ambient,
						true
					);
				}
				else {
					// add a wall above the portal
					mesh->addRectangle(this, wall,
						m_staticMeshFloorAltitude,
						m_staticMeshCeilingAltitude,
						mirror->m_tex[DFWALL_TEXTURE_MID],
						m_ambient,
						false
					);
				}
			}
			else {
				// for any other elevators (basic, inv, floor) force he data provided
				mesh->addRectangle(this, wall,
					bottom,
					top,
					mirror->m_tex[what],
					m_ambient,
					false
				);
			}
		}
	}

	// only build top and bottom for vertical elevators (sliding ones : spin1 are not needed)
	if (!((int)flags & (int)dfWallFlag::MORPHS_WITH_ELEV)) {
		mesh->addFloor(polygons(1), bottom, m_ceilingTexture, m_ambient, clockwise);
		mesh->addFloor(polygons(1), top, m_floorTexture, m_ambient, clockwise);
	}

	// add deferred signs on the elevators (the sign is physically on an elevator)
	// centerVertices the level space into the model space
	for (auto wall : m_deferedSigns) {
		dfSector* sector = wall->sector();
		float translate = m_staticMeshFloorAltitude - sector->m_staticMeshFloorAltitude;
		float height = sector->m_staticMeshCeilingAltitude - sector->m_staticMeshFloorAltitude;
		dfLogicTrigger* trigger = addSign(mesh, wall, bottom - translate, bottom - translate + height, DFWALL_TEXTURE_BOTTOM);
		if (parent != nullptr) {
			parent->addChild(trigger);
		}
	}
}

/**
 * Test if the sector is displayed on screen (the parent super-sector is visible)
 */
bool dfSector::visible(void)
{
	return m_super->visible();
}

/**
 * Register a trigger to the sector for backward parsing
 */
void dfSector::addTrigger(dfLogicTrigger* trigger)
{
	m_triggers.push_back(trigger);
}
/**
 * register a program for the sector
 */
void dfSector::addProgram(DarkForces::InfProgram* program)
{
	program->compile();
	addComponent(program);
	m_programs.push_back(program);	// keep track to delete the program when needed
}

/**
 * Update the AABB box
 */
void dfSector::setAABBtop(float z_level)
{
	m_worldAABB.m_p1.y = z_level / 10.0f;	// convert to opengl space
	if (m_super) {
		m_super->extendAABB(m_worldAABB);
	}
}

/**
 * Update the AABB box
 */
void dfSector::setAABBbottom(float z_level)
{
	m_worldAABB.m_p.y = z_level / 10.0f;	// convert to opengl space
	if (m_super) {
		m_super->extendAABB(m_worldAABB);
	}
}

dfSector::~dfSector()
{
	for (auto wall: m_walls) {
		delete wall;
	}

	for (auto program : m_programs) {
		delete program;
	}
}