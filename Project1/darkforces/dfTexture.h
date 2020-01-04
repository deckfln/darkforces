#pragma once

#include <string>

struct dfTexture {
	std::string m_name;
	int height = 0;
	int width = 0;
	int nrChannels = 0;

	float m_xoffset = 0;	// start x offset in the megatexture
	float m_yoffset = 0;	// start y offset in the megatexture
	float m_mega_width = 0;	// end x offset in the megatexture
	float m_mega_height = 0;	// end y offset in the megatexture

	int bsize = 0;			// size in block 16x16

	unsigned char* data;	// source data
};
