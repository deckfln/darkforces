#include <list>

#include "dfAtlasTexture.h"
#include "dfBitmap.h"
#include "../framework/fwTexture.h"
#include "../framework/fwUniform.h"
#include "../framework/fwMaterialBasic.h"

/**
 * basic algorithm : use a square placement map, find an empty spot, store the texture. if no spot can be found, increase the texture size
 */
dfAtlasTexture::dfAtlasTexture(std::vector<dfBitmapImage*>& images)
{
	std::list<dfBitmapImage*> sorted_textures;
	const int blockSize = 4;
	int rgba=3;

	// count number of 16x16 blocks
	int blocks4x4 = 0;
	int bx, by;

	for (auto texture : images) {
		if (texture) {
			rgba = texture->m_nrChannels;

			texture->boardSize(blockSize);
			blocks4x4 += texture->m_bsize;

			sorted_textures.push_back(texture);
		}
	}

	// sort textures by size : big first
	sorted_textures.sort([](dfBitmapImage* a, dfBitmapImage* b) { return a->m_bsize > b->m_bsize; });

	// evaluate the size of the megatexture (square texture of 4x4 blocks)
	int bsize = (int)ceil(sqrt(blocks4x4));

	// try to place all objects on the map.
	// if fail, increase the size of the map until we place all textures

	bool allplaced = false;
	bool *placement_map;
	int size;

	// register the position of the texture on the atlas texture
	std::vector<glm::ivec2> texture_position;
	texture_position.resize(sorted_textures.size());

	// place the texture on a 4x4 board
	do {
		// avaibility map of 16x16 blocks : false = available, true = used
		placement_map = new bool[bsize * bsize]();

		// find an available space on the map
		int px, py;
		int c = 0;
		glm::ivec2 first_available_position = glm::ivec2(0);
		int first_available_byte = 0;

		allplaced = true;	// suppose we will be able to fit all textures

		int iTex = 0;
		for (auto texture : sorted_textures) {
			bx = texture->m_bsizeWidth;
			by = texture->m_bsizeHeight;

			// find a spot of the placement map
			px = first_available_position.x;
			py = first_available_position.y;

			first_available_byte = first_available_position.x + first_available_position.y * bsize;
			bool ok = false;

			for (auto i = 0; i < bsize * bsize; i++) {
				// test each position to find one we can start checking
				if (!placement_map[py * bsize + px]) {
					first_available_position.x = px;
					first_available_position.y = py;
					ok = true;	// suppose we have a spot

					for (auto y = py; y < py + by; y++) {
						if (y >= bsize) {
							ok = false;
							break;
						}
						// check availability on the X axis
						for (auto x = px; x < px + bx; x++) {
							if (x >= bsize) {
								ok = false;
								break;
							}
							if (placement_map[y * bsize + x]) {
								ok = false;	// actually, no we don't have a spot
								break;
							}
						}
						if (!ok) {
							break;
						}
					}
				}

				// found a spot
				if (ok) {
					break;
				}

				// progress along the placement map. 
				px++;
				if (px >= bsize - bx) {
					px = 0;
					py++;
					if (py >= bsize - by) {
						break;
					}
				}
			}

			// was not able to find a spot.
			if (!ok) {
				// need to increase the size of the map
				delete[] placement_map;
				allplaced = false;
				bsize++;
				break;
			}
			c++;

			// mark the object on the placement map
			for (auto y = py; y < py + by; y++) {
				for (auto x = px; x < px + bx; x++) {
					placement_map[y * bsize + x] = true;
				}
			}

			texture_position[iTex].x = px;
			texture_position[iTex].y = py;
			iTex++;
		}
	} while (!allplaced);

	delete[] placement_map;

	// store the texture in the atlas
	// megatexture in pixel (1 block = 4 pixels)
	size = bsize * blockSize;
	m_megatexture = new unsigned char[size * size * rgba]();

	int iTex = 0;
	int x, y;
	for (auto texture: sorted_textures) {
		// extract the position on the 4x4 board
		x = texture_position[iTex].x * blockSize;
		y = texture_position[iTex].y * blockSize;

		texture->copyTo(m_megatexture, x, y, size, rgba);

		iTex++;
	}

	// create the fwTexture
	m_fwtextures = new fwTexture(m_megatexture, size, size, rgba, GL_NEAREST);

	// and the index
	m_megatexture_idx.resize(images.size());

	int i = 0;
	for (auto texture : images) {
		if (texture) {
			m_megatexture_idx[i] = glm::vec4(texture->m_xoffset, texture->m_yoffset, texture->m_mega_width, texture->m_mega_height);
			texture->m_textureID = i;
			i++;
		}
	}
	m_shader_idx = new fwUniform("megatexture_idx", &m_megatexture_idx[0], i);
}

/**
 * Bind the atlas texture to a material
 */
void dfAtlasTexture::bindToMaterial(fwMaterial* material)
{
	if (material->type(BASIC_MATERIAL)) {
		((fwMaterialBasic* )material)->addDiffuseMap(m_fwtextures);
	}
	material->addUniform(m_shader_idx);
}

/**
 * Save the texture as a PNG
 */
void dfAtlasTexture::save(std::string file)
{
	m_fwtextures->save(file);
}

/**
 *
 */
fwTexture* dfAtlasTexture::texture(void)
{
	return m_fwtextures;
}

dfAtlasTexture::~dfAtlasTexture()
{
	delete m_megatexture;
	delete m_fwtextures;
	delete m_shader_idx;
}
