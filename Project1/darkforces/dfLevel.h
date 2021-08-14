#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>
#include <queue>

#include "../framework/fwAABBox.h"
#include "../framework/fwCamera.h"
#include "../framework/fwScene.h"
#include "../framework/fwMaterialBasic.h"

#include "dfSector.h"
#include "dfSuperSector.h"
#include "dfBitmap.h"
#include "dfParseINF.h"
#include "dfElevator.h"
#include "dfFileGOB.h"

class fwCylinder;

class dfMesh;
class dfParserObjects;
class dfAtlasTexture;
class dfElevator;
class dfPalette;
class gaCollisionPoint;

/**
 *
 */
class dfLevel
{
	std::string m_name;
	std::string m_level;
	dfPalette* m_palette = nullptr;				// color palette
	std::map<std::string, dfSector *> m_sectorsName;	// all sectors of the level by Name
	std::vector<dfSector*> m_sectorsID;			// all sectors of the level by ID
	std::vector<dfBitmap*> m_bitmaps;			// all textures of the level

	std::map<std::string, dfElevator*> m_elevators;	// all elevators of the sector

	glm::vec3 m_skyTexture;						// Identify texture for sectors FLAGS1 = 1
	float m_skyAltitude=0;						

	fwAABBox m_boundingBox;						// bounding box of the level

	std::vector<dfBitmapImage *> m_allTextures;	// textures of the level based on the index from LEV structure
	std::list<dfSuperSector *> m_supersectors;	// space partioning of sectors

	// Counters
	int m_currentBitmap = 0;
	int m_currentTexture = 0;

	/*
	unsigned char* m_megatexture = nullptr;		// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures;					// fwTexture for the megatextures
	std::vector<glm::vec4> m_megatexture_idx;	// rg = texture start  ba = texture size
	fwUniform* m_shader_idx = nullptr;
	*/

	dfAtlasTexture* m_atlasTexture = nullptr;	// store all textures.gob used in the level
	dfAtlasTexture* m_sprites = nullptr;		// store all sprites.gob
	fwMaterialBasic* m_material = nullptr;

	fwSkyline* m_skybox = nullptr;				// sky texture
	bool m_skyboxAdded = false;

	dfSector* m_lastSector = nullptr;			// cached sector from the last findSector
	dfSuperSector* m_lastSuperSector = nullptr;	// cached super sector from the last findSector

	dfParseINF* m_inf = nullptr;				// level logic retrieved from the INF file
	std::list <dfLogicTrigger*> m_triggers;		// all triggers of the level

	dfParserObjects* m_objects = nullptr;		// all objects in the level

	void loadBitmaps(dfFileSystem* fs, std::string file);
	void spacePartitioning(void);
	void buildGeometry(void);
	dfSuperSector* findSuperSector(glm::vec3& position);
	void initElevators(void);
	void convertDoors2Elevators(void);
	void createTriggers(void);
	void createTriggerForElevator(dfElevator *elevator);

public:
	dfLevel(dfFileSystem* fs, std::string file);
	dfSector* findSector(const glm::vec3& position);			// find sector by GL coordinate
	dfSector* findSector(char *name);							// find sector by name
	dfSector* findSector(const std::string& name);				// find sector by name
	dfSector* findSectorLVL(const glm::vec3& level_position);	// find sector by level coordinate
	void testSwitch(fwAABBox& player, gaEntity* source);
	void draw(fwCamera* camera, fwScene* scene);
	std::vector<dfSector*>& sectorsID(void) { return m_sectorsID; };
	std::vector<dfBitmap*>& textures(void) { return m_bitmaps; };
	dfPalette* palette(void) { return m_palette; };
	static void level2gl(const glm::vec3& source, glm::vec3& target);
	static void level2gl(glm::vec3& source);
	static void level2gl(fwCylinder& source, fwCylinder& target);
	static void gl2level(const glm::vec3& gl, glm::vec3& level);
	static void gl2level(fwCylinder& gl, fwCylinder& level);
	~dfLevel();
};