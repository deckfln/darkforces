#pragma once
#include <string>
#include <vector>
#include <list>

#include "dfSector.h"

struct dfTexture {
	std::string m_name;
	int height;
	int width;
	int nrChannels;

	int m_texture;		// texture index (64x64, 64x128 ....)
	float m_xoffset;	// x offset in the megatexture
	float m_yoffset;	// y offset in the megatexture

	unsigned char* data;	// source data
};

class dfLevel
{
	std::string m_name;
	std::string m_level;
	std::vector<dfSector *> m_sectors;
	std::vector<dfTexture *> m_textures;

	int m_currentTexture = 0;

	fwGeometry* m_geometry = nullptr;
	glm::vec3* m_vertices = nullptr;

	unsigned char* m_megatexture[4];	// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures[4];			// fwTexture for the megatextures

	void convert2geometry(void);
	void loadGeometry(std::string file);
	void compressTextures(void);

public:
	dfLevel(std::string name);
	fwGeometry* geometry(void) { return m_geometry; };
	fwTexture* megatexture(int i) { return m_fwtextures[i]; };
	~dfLevel();
};