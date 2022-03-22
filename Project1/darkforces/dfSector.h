#pragma once
#include <string>
#include <vector>
#include <list>

#include "dfWall.h"
#include "dfFileGOB.h"

using Coord = float;
using Point = std::array<Coord, 2>;

#include "../framework/math/fwSphere.h"
#include "../framework/math/fwPolygon2D.h"

#include "../gaEngine/AABBoxTree.h"
#include "../gaEngine/gaMessage.h"
#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaComponent/gaSound.h"
#include "../gaEngine/gaVolumeSpace.h"
#include "../gaEngine/gaVoxelSpace.h"

#include "dfComponent/InfStandardTrigger.h"

#include "flightRecorder/frSector.h"

class fwCylinder;
class gaCollisionPoint;
class dfMesh;
class dfSuperSector;
class dfLogicTrigger;
class dfLevel;

namespace DarkForces {
	namespace Component {
		class InfElevator;
	}
}

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
enum dfSectorFlag {
	// Flag 1
	EXTERIOR_NO_CEIL = 1,				// (SKY) Note: actual ceiling limit will be the ceiling altitude + 100 
	DOOR = 2,							// instant door 
	SHOT_REFLECTION = 4,				// MAG.SEAL walls, floorand ceiling reflect weapon shots 
	EXTERIOR_ADJOIN = 8,				// will adjoin adjacent skies 
	ICE_FLOOR = 16,					// (SKATING)
	SNOW_FLOOR = 32,					// no apparent effects 
	EXPLODING_WALL_DOOR  = 64,		// instant exploding door 
	EXTERIOR_NO_FLOOR = 128,			// (PIT) Note : actual floor limit will be the floor altitude - 100 
	EXTERIOR_FLOOR_ADJOIN = 256,		// will adjoin adjacent pits 
	CRUSHING_SECTOR = 512,			// vertically moving elevators will crush the player 
	NO_WALL_DRAW = 1024,				//  "HORIZON" removes walls of a sector(sector must be sky and pit to work properly) 
	LOW_DAMAGE = 2048,
	HIGH_DAMAGE = 4096,				//	both can be combined for GAS 
	NO_SMART_OBJECT_REACTION = 8192,	//
	SMART_OBJECT_REACTION = 16384,
	SUBSECTOR = 32768,				// no apparent effects 
	SAFE_SECTOR = 65536,
	RENDERED = 131072,
	PLAYER = 262144,
	SECRET_SECTOR = 524288,			// increments the % secret when entered
};

class dfSector : public gaEntity
{
	dfLevel* m_level = nullptr;

	int32_t m_layer = -1;								// floor number

	float m_staticMeshFloorAltitude = 0;				// floor altitude for the superSector static mesh
	float m_staticMeshCeilingAltitude = 0;				// ceiling altitude for the superSector static mesh

	float m_referenceFloorAltitude = 0;					// reference floor altitude from the INF file
	float m_referenceCeilingAltitude = 0;				// reference ceiling altitude from the INF file

	float m_floorAltitude = 0;							// current floor altitude from the INF file
	float m_ceilingAltitude = 0;						// current ceiling altitude from the INF file

	std::list <dfLogicTrigger*> m_remoteTriggers;		// list of triggers on other sector with impact on that one
	std::list <DarkForces::Component::InfStandardTrigger*> m_programs;		// triggers standard bound to the sector
	std::list <DarkForces::Component::InfElevator*> m_elevators;	// elevator(s) bound to the sector

	int m_eventMask = 0;								// events triggering messages
	gaMessage m_message;								// message to send
	std::vector <struct dfVerticeConnexion> m_verticeConnexions;	// get the vertice to the right and the left of each vertice
	std::vector<std::vector<Point>> m_polygons_vertices;// polylines enclosing the sector : [0] external polygon, [1+] internal holes : by vertices
	Framework::Primitive::Polygon2D m_2Dpolygon;		// polylines of the sector

	std::vector<std::vector<dfWall*>> m_polygons_walls;	// polylines enclosing the sector : [0] external polygon, [1+] internal holes : by walls
	int m_displayPolygons = 0;							// default number of polygon to draw 0=ALL, 1 = external one, 2 = first hole
	int m_firstVertex = 0;								// position of the first sector vertice in the super-sector vertices
	int m_nbVertices = 0;
	int m_wallVerticesStart = 0;						// position of the first wall vertice in the super-sector vertices
	int m_wallVerticesLen = 0;
	int m_floorVerticesStart = 0;						// position of the first floor vertice in the super-sector vertices
	int m_floorVerticesLen = 0;
	std::list <dfWall*>m_deferedSigns;					// list of signs to add later (likely when the sector is an elevator)
	std::vector<dfSector *> m_dummy;
	std::vector<dfSector *> &m_sectorsID = m_dummy;		// all neighbour sectors of the level by ID

	std::vector<dfSector*> m_includes;					// list of sectors included in the current one
	dfSector* m_includedIn = nullptr;					// if the sector is included in another one

	float m_ambient = 0;								// DarkForces light (0..31)
	float m_currentAmbient = 0;							// OpenGL light (0..1)

	bool m_visited = false;								// did the player visit the sector (for the automap)

	std::stack<float> m_ambients;						// previous lightning

	dfSuperSector* m_super = nullptr;					// link back to the supersector
	std::vector<uint32_t> m_portalWall;					// wall used as a portal to another sector
	std::vector<uint32_t> m_mirrorWall;					// wall used as a portal to another sector

	uint32_t m_soundVolumeID;							// sound volume based on the sector
	GameEngine::Component::Sound m_sound;               // to play sound when a laser hit the wall

	std::vector<gaEntity*> m_3DObjects;					// 3D objects stored in the sector (to deal with wakeup messages)

	void buildWalls(dfMesh* mesh, dfWallFlag displayPolygon);
	void buildFloorAndCeiling(dfMesh* mesh);
	void buildSigns(dfMesh* mesh);
	dfLogicTrigger* addSign(dfMesh *mesh, dfWall* wall, float z, float z1, int texture);

public:
	GameEngine::AABBoxTree m_worldAABB;					// AABB node to be inserted in a bigger tree

	int m_id = -1;

	// original values
	float m_height = 0;									// height of the sector

	glm::vec3 m_floorTexture;
	glm::vec3 m_ceilingTexture;

	unsigned int m_flag1 = 0;
	unsigned int m_flag2 = 0;
	unsigned int m_flag3 = 0;

	// local data in space world
	std::vector <dfWall*> m_walls;		// modified walls
	std::vector <glm::vec2> m_vertices;

	std::list <int> m_portals;	// sectorID of the portals

	dfSector(std::istringstream& infile, std::vector<dfSector*>& sectorsID, dfLevel *level);
	dfSector(dfSector*);

	void setTriggerFromWall(dfLogicTrigger* trigger);
	void setTriggerFromFloor(dfLogicTrigger* trigger);
	void setTriggerFromSector(dfLogicTrigger* trigger);

	bool inAABBox(const glm::vec3& position);						// quick test point inside AABB
	bool collideAABB(const fwAABBox& box);							// quick test to find AABB collision
	bool AABBcollide(const fwAABBox& box);							// if the AABB collide with the sector

	// overriden function
	bool isPointInside(const glm::vec3& position) override;
	bool isPointInside(const glm::vec3& position, bool fullTest);


	// getter/setter
	inline dfSuperSector* supersector(void) { return m_super; };
	inline void supersector(dfSuperSector* s) { m_super = s; };
	inline uint32_t portalWall(uint32_t portalID) { return m_portalWall[portalID]; };
	inline uint32_t portalMirror(uint32_t portalID) { return m_mirrorWall[portalID]; };
	inline void soundVolume(uint32_t volume) { m_soundVolumeID = volume; };
	inline uint32_t soundVolume(void) { return m_soundVolumeID; };
	inline float ambient(void) { return m_ambient; };
	inline float ceiling(void) { return m_referenceCeilingAltitude; };
	inline float currentFloorAltitude(void) { return m_floorAltitude; };
	inline float referenceFloor(void) { return m_referenceFloorAltitude; };
	inline float referenceCeiling(void) { return m_referenceCeilingAltitude; };
	inline dfSector* isIncludedIn(void) { return m_includedIn; };
	inline float height(void) { return m_height; };
	inline unsigned flag(void) { return m_flag1; };
	inline bool flag(int v) { return (m_flag1 & v) != 0; };
	inline void eventMask(int eventMask) { m_eventMask = eventMask; };
	inline void wallVertices(int start, int len) { m_wallVerticesStart = start, m_wallVerticesLen = len; };
	inline void floorVertices(int start, int len) { m_floorVerticesStart = start, m_floorVerticesLen = len; };
	inline int firstVertex(void) { return m_firstVertex; };
	inline int nbVertices(void) { return m_nbVertices; };
	inline void add3Dobject(gaEntity* entity) { m_3DObjects.push_back(entity); };
	inline const uint32_t layer(void) { return m_layer; };
	void wallCenter(uint32_t wallID, glm::vec3& center, float &surface);			// fill the 3D center of the wall

	void ceiling(float z);

	void currentFloorAltitude(float z);

	void staticFloorAltitude(float z);
	float staticFloorAltitude(void);

	void staticCeilingAltitude(float z);
	float staticCeilingAltitude(void);

	std::vector <dfWall*>& walls(dfWallFlag);

	std::vector<std::vector<Point>>& polygons(int displayPolygon);

	void addObject(dfMesh* object);
	float boundingBoxSurface(void);
	void linkWalls(void);
	void buildElevator(gaEntity*parent, dfMesh *mesh, float bottom, float top, int what, bool clockwise, dfWallFlag flags);
	dfMesh* buildElevator_new(float bottom, float top, int what, bool clockwise, dfWallFlag flags, std::list<dfLogicTrigger*>& m_signs);

	bool includedIn(dfSector* sector);

	void bindWall2Sector(void);

	void event(int event_mask);
	void removeHollowWalls(void);


	void changeAmbient(float ambient);								// set ambient lightning of the sector
	void addToAmbient(float delta);									// add/substract from the current lighning
	void popAmbient(void);										// restore previous ambient lighning
	void buildGeometry(dfMesh *mesh, dfWallFlag);
	void voxelisation(GameEngine::VoxelSpace<dfSector*>& voxels);	// voxelize the current sector

	bool visible(void);												// get current visibility of the sector
	void addProgram(DarkForces::Component::InfStandardTrigger*);	// register a INF trigger standard on the sector
	void addElevator(DarkForces::Component::InfElevator* elevator); // register a INF elevator on the sector
	void setAABBtop(float z_level);
	void setAABBbottom(float z_level);

	void dispatchMessage(gaMessage* message) override;				// let an entity deal with a situation

	// debugger
	inline int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::Sector);
	}																// size of one record
	uint32_t recordState(void* record) override;					// return a record of the entity state (for debug)
	void loadState(void* record) override;							// reload an entity state from a record
	void debugGUIChildClass(void) override;							// Add dedicated component debug the entity

	~dfSector();
};