#pragma once
#include <string>
#include <vector>
#include <list>

#include "dfSector.h"
#include "../framework/fwAABBox.h"

struct dfTexture {
	std::string m_name;
	int height=0;
	int width=0;
	int nrChannels=0;

	float m_xoffset = 0;	// start x offset in the megatexture
	float m_yoffset = 0;	// start y offset in the megatexture
	float m_mega_width = 0;	// end x offset in the megatexture
	float m_mega_height = 0;	// end y offset in the megatexture

	int bsize = 0;			// size in block 16x16

	unsigned char* data;	// source data
};

struct dfLayer {
	fwAABBox m_boundingBox;
	std::list <dfSector*> m_sectors;
};

class dfLevel
{
	std::string m_name;
	std::string m_level;
	std::vector<dfSector *> m_sectors;		// all sectors of the level
	std::vector<dfTexture *> m_textures;	// all textures of the level
	std::vector<dfLayer> m_layers;	// space partioning of sectors

	int m_currentTexture = 0;

	std::vector <glm::vec3> m_vertices;		// level vertices
	std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
	std::vector <UINT> m_textureID;		// TextureID inside the megatexture

	fwGeometry* m_geometry = nullptr;

	unsigned char* m_megatexture = nullptr;	// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures;				//fwTexture for the megatextures
	std::vector<glm::vec4> m_megatexture_idx;// rg = texture start  ba = texture size
	fwUniform* m_shader_idx = nullptr;

	void buildWalls(void);
	void buildFloor(void);
	void buildGeometry(void);
	void loadBitmaps(std::string file);
	void buildAtlasMap(void);
	void spacePartitioning(void);
	void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, int texture);

public:
	dfLevel(std::string name);
	fwGeometry* geometry(void) { return m_geometry; };
	fwTexture* texture() { return m_fwtextures; };
	fwUniform* index(void) { return m_shader_idx; };
	dfSector* findSector(glm::vec3& position);
	~dfLevel();
};