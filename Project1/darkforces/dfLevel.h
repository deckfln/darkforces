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
#include "dfTexture.h"
#include "dfParseINF.h"

/**
 *
 */
class dfLevel
{
	std::string m_name;
	std::string m_level;
	std::vector<dfSector *> m_sectors;			// all sectors of the level
	std::vector<dfTexture *> m_textures;		// all textures of the level
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

	void loadBitmaps(std::string file);
	void buildAtlasMap(void);
	void spacePartitioning(void);
	void buildGeometry(void);
	dfSuperSector* findSuperSector(glm::vec3& position);

public:
	dfLevel(std::string name);
	dfSector* findSector(glm::vec3& position);
	void draw(fwCamera* camera, fwScene* scene);
	~dfLevel();
};