#pragma once

#include <vector>
#include <string>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

class fwTexture;
class fwUniform;
class fwMaterial;

namespace Framework {
	class TextureAtlas
	{
		std::vector<fwTexture*> m_source;			// source data
		std::vector<uint8_t> m_megatexture;			// raw data 64x64, 64x128, 64x256, 64x512
		fwTexture* m_fwtextures = nullptr;			// fwTexture for the megatextures
		std::vector<glm::vec4> m_megatexture_idx;	// rg = texture start  ba = texture size
		std::vector<glm::ivec2> m_megatexture_size;	// x,y = size of each picture in pixel
		fwUniform* m_shader_idx = nullptr;			// store the position of each teture in the atlas
		int m_width = 0, m_height = 0;
		int m_nrChannels = 0;

	public:
		TextureAtlas(void);
		void add(fwTexture*);
		fwTexture* generate(void);
		void bindToMaterial(fwMaterial* material, std::string uniform = "");
		void save(const std::string& file);
		void texel(uint32_t index, glm::vec4& texel);	// extract the texture coordinates of the image at index 
		fwTexture* texture(void);
		~TextureAtlas(void);
	};
}
