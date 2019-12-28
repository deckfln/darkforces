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

	float m_xoffset = 0;	// start x offset in the megatexture
	float m_yoffset = 0;	// start y offset in the megatexture
	float m_x1offset = 0;	// end x offset in the megatexture
	float m_y1offset = 0;	// end y offset in the megatexture

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

	unsigned char* m_megatexture;	// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures;			// fwTexture for the megatextures

	void convert2geometry(void);
	void loadGeometry(std::string file);
	void compressTextures(void);

public:
	dfLevel(std::string name);
	fwGeometry* geometry(void) { return m_geometry; };
	fwTexture* megatexture(int i) { return m_fwtextures; };
	~dfLevel();
};