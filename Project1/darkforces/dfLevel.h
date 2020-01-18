#pragma once
#include <string>
#include <vector>
#include <list>
#include <map>

#include "../framework/fwAABBox.h"
#include "../framework/fwCamera.h"
#include "../framework/fwScene.h"
#include "../framework/fwMaterialBasic.h"

#include "dfSector.h"
#include "dfSuperSector.h"
#include "dfBitmap.h"
#include "dfParseINF.h"
#include "dfLogicElevator.h"
#include "dfFileGOB.h"
#include "dfPalette.h"

/**
 *
 */
class dfLevel
{
	std::string m_name;
	std::string m_level;
	dfPalette* m_palette = nullptr;				// color palette
	std::vector<dfSector *> m_sectors;			// all sectors of the level
	std::vector<dfBitmapImage *> m_textures;	// all textures of the level
	std::list<dfSuperSector *> m_supersectors;	// space partioning of sectors

	std::map<std::string, dfSector*> m_hashSectors;		// dictionnaries of sectors

	int m_currentTexture = 0;

	unsigned char* m_megatexture = nullptr;		// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures;					// fwTexture for the megatextures
	std::vector<glm::vec4> m_megatexture_idx;	// rg = texture start  ba = texture size
	fwUniform* m_shader_idx = nullptr;
	
	fwMaterialBasic* m_material = nullptr;

	dfSector* m_lastSector = nullptr;			// cached sector from the last findSector
	dfSuperSector* m_lastSuperSector = nullptr;	// cached super sector from the last findSector

	dfParseINF* m_inf = nullptr;				// level logic retrieved from the INF file
	std::list<dfLogicElevator*> m_activeElevators;// elevators currently moving on the level
	std::list <dfLogicTrigger*> m_triggers;		// all triggers of the level

	void loadBitmaps(dfFileGOB* gob, std::string file);
	void buildAtlasMap(void);
	void spacePartitioning(void);
	void buildGeometry(void);
	dfSuperSector* findSuperSector(glm::vec3& position);
	void initElevators(void);
	void convertDoors2Elevators(void);
	void createTriggerForSpin(void);

public:
	dfLevel(dfFileGOB* dark, dfFileGOB* textures, std::string file);
	dfSector* findSector(glm::vec3& position);
	void testSwitch(fwAABBox& player);
	void draw(fwCamera* camera, fwScene* scene);
	std::vector<dfSector*>& sectors(void) { return m_sectors; };
	std::vector<dfBitmapImage*>& textures(void) { return m_textures; };
	void activateElevator(dfLogicElevator* elevator) { m_activeElevators.push_back(elevator); };
	void deactivateElevator(dfLogicElevator* elevator) { m_activeElevators.remove(elevator); };
	void animate(time_t delta);
	~dfLevel();
};