#pragma once

#include <vector>
#include <string>
#include <glm/vec4.hpp>

class dfBitmapImage;
class fwTexture;
class fwUniform;
class fwMaterialBasic;

class dfAtlasTexture
{
	unsigned char* m_megatexture = nullptr;		// raw data 64x64, 64x128, 64x256, 64x512
	fwTexture* m_fwtextures;					// fwTexture for the megatextures
	std::vector<glm::vec4> m_megatexture_idx;	// rg = texture start  ba = texture size
	fwUniform* m_shader_idx = nullptr;			// store the position of each teture in the atlas
	int m_width=0, m_height=0;
	int m_nrChannels = 0;

public:
	dfAtlasTexture(std::vector<dfBitmapImage*>& images);
	void bindToMaterial(fwMaterialBasic* material);
	void save(std::string file);
	~dfAtlasTexture();
};