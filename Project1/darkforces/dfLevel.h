#pragma once
#include <string>
#include <vector>
#include <list>

#include "dfSector.h"

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

class dfLevel
{
	std::string m_name;
	std::string m_level;
	std::vector<dfSector *> m_sectors;
	std::vector<dfTexture *> m_textures;

	int m_currentTexture = 0;

	std::vector <glm::vec3> m_vertices;		// level vertices
	std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
	std::vector <UINT> m_textureID;		// TextureID inside the megatexture

	fwGeometry* m_geometry = nullptr;

	unsigned char* m_megatexture = nullptr;	// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures;				//fwTexture for the megatextures
	std::vector<glm::vec4> m_megatexture_idx;// rg = texture start  ba = texture size
	fwUniform* m_shader_idx = nullptr;

	void convert2geometry(void);
	void loadGeometry(std::string file);
	void compressTextures(void);
	void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, int texture);

public:
	dfLevel(std::string name);
	fwGeometry* geometry(void) { return m_geometry; };
	fwTexture* texture() { return m_fwtextures; };
	fwUniform* index(void) { return m_shader_idx; };
	~dfLevel();
};