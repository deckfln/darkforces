#pragma once
#include <string>
#include <vector>
#include <list>

#include "dfWall.h"
#include "dfFileGOB.h"

using Coord = float;
using Point = std::array<Coord, 2>;

#include "../framework/fwAABBox.h"
#include "../framework/math/fwSphere.h"

#include "dfLogicTrigger.h"

class dfMesh;
class dfSuperSector;

/**
 * Connection of each vertice to the left and the right
 */
struct dfVerticeConnexion {
	int m_leftVertice = -1;	// index of the vertice on the left
	int m_rightVertice = -1;

	int m_leftWall = -1;	// index of the wall on the left
	int m_rightWall = -1;

	bool parsed = false;
};

// Flag1 values
enum {
	DF_SECTOR_EXTERIOR_NO_CEIL = 1,				// (SKY) Note: actual ceiling limit will be the ceiling altitude + 100 
	DF_SECTOR_DOOR = 2,							// instant door 
	DF_SECTOR_SHOT_REFLECTION = 4,				// MAG.SEAL walls, floorand ceiling reflect weapon shots 
	DF_SECTOR_EXTERIOR_ADJOIN = 8,				// will adjoin adjacent skies 
	DF_SECTOR_ICE_FLOOR = 16,					// (SKATING)
	DF_SECTOR_SNOW_FLOOR = 32,					// no apparent effects 
	DF_SECTOR_EXPLODING_WALL_DOOR  = 64,		// instant exploding door 
	DF_SECTOR_EXTERIOR_NO_FLOOR = 128,			// (PIT) Note : actual floor limit will be the floor altitude - 100 
	DF_SECTOR_EXTERIOR_FLOOR_ADJOIN = 256,		// will adjoin adjacent pits 
	DF_SECTOR_CRUSHING_SECTOR = 512,			// vertically moving elevators will crush the player 
	DF_SECTOR_NO_WALL_DRAW = 1024,				//  "HORIZON" removes walls of a sector(sector must be sky and pit to work properly) 
	DF_SECTOR_LOW_DAMAGE = 2048,
	DF_SECTOR_HIGH_DAMAGE = 4096,				//	both can be combined for GAS 
	DF_SECTOR_NO_SMART_OBJECT_REACTION = 8192,	//
	DF_SECTOR_SMART_OBJECT_REACTION = 16384,
	DF_SECTOR__SUBSECTOR = 32768,				// no apparent effects 
	DF_SECTOR__SAFE_SECTOR = 65536,
	DF_SECTOR__RENDERED = 131072,
	DF_SECTOR__PLAYER = 262144,
	DF_SECTOR__SECRET_SECTOR = 524288			// increments the % secret when entered
};

class dfSector
{
	std::list <dfLogicTrigger*> m_triggers;		// list of all triggers on the sector.
	int m_eventMask = 0;						// events triggering messages
	dfMessage m_message;						// message to send
	std::vector <struct dfVerticeConnexion> m_verticeConnexions;	// get the vertice to the right and the left of each vertice
	std::vector<std::vector<Point>> m_polygons_vertices;			// polylines enclosing the sector : [0] external polygon, [1+] internal holes : by vertices
	std::vector<std::vector<dfWall*>> m_polygons_walls;				// polylines enclosing the sector : [0] external polygon, [1+] internal holes : by walls
	int m_displayPolygons = 0;										// defualt number of polygon to draw 0=ALL, 1 = external one, 2 = first hole

public:
	fwAABBox m_boundingBox;

	std::string m_name = "";
	int m_id = -1;
	int m_layer = -1;
	float m_ambient = 0;

	// current values
	float m_floorAltitude = 0;
	float m_ceilingAltitude = 0;

	// original values
	float m_height = 0;				// height of the sector

	glm::vec3 m_floorTexture;
	glm::vec3 m_ceilingTexture;

	unsigned int m_flag1 = 0;
	unsigned int m_flag2 = 0;
	unsigned int m_flag3 = 0;

	// local data in space world
	std::vector <dfWall*> m_walls;		// modified walls
	std::vector <glm::vec2> m_vertices;

	std::list <int> m_portals;	// sectorID of the portals

	// same data but in the supersector (opengl space)
	dfSuperSector* m_super = nullptr;

	dfSector(std::istringstream& infile);
	void setTriggerFromWall(dfLogicTrigger* trigger);
	void setTriggerFromFloor(dfLogicTrigger* trigger);
	void setTriggerFromSector(dfLogicTrigger* trigger);

	bool inAABBox(glm::vec3& position) { return m_boundingBox.inside(position); };

	void floor(float z);
	void ceiling(float z);

	float ceiling(void) { return m_ceilingAltitude; };
	void parent(dfSuperSector* parent) { m_super = parent; };
	float height(void) { return m_height; };
	unsigned flag(void) { return m_flag1; };
	void eventMask(int eventMask) { m_eventMask = eventMask; };
	std::vector <dfWall*>& walls(int displayPolygon = -1);

	std::vector<std::vector<Point>>& polygons(int displayPolygon);

	void addObject(dfMesh* object);
	bool isPointInside(glm::vec3& position, bool fullTest);
	float boundingBoxSurface(void);
	void setFloor(float floor);
	void updateVertices(void);
	void linkWalls(void);
	void buildElevator(dfMesh *mesh, float bottom, float top, int what, bool clockwise, int flags);
	void buildFloor(dfMesh* mesh);

	void bindWall2Sector(std::vector <dfSector*> sectors);

	void event(int event_mask);
	void removeHollowWalls(void);
	bool checkCollision(float step, glm::vec3& position, float radius, glm::vec3& collision);

	~dfSector();
};