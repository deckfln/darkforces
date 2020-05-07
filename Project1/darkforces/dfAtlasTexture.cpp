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

			bx = ceil(texture->m_width / blockSize);	// round up to 4x4
			by = ceil(texture->m_height / blockSize);
			texture->bsize = bx * by;

			blocks4x4 += texture->bsize;

			sorted_textures.push_back(texture);
		}
	}

	// sort textures by size : big first
	sorted_textures.sort([](dfBitmapImage* a, dfBitmapImage* b) { return a->bsize > b->bsize; });

	// evaluate the size of the megatexture (square texture of 4x4 blocks)
	int bsize = (int)ceil(sqrt(blocks4x4));

	// try to place all objects on the map.
	// if fail, increase the size of the map until we place all textures

	bool allplaced = false;
	bool* placement_map;
	int size;

	do {
		// avaibility map of 16x16 blocks : false = available, true = used
		placement_map = new bool[bsize * bsize]();

		// megatexture in pixel (1 block = 4 pixels)
		size = bsize * blockSize;
		m_megatexture = new unsigned char[size * size * rgba];

		// parse textures and place them on the megatexture
		// find an available space on the map
		int px, py;
		int c = 0;

		allplaced = true;	// suppose we will be able to fit all textures

		for (auto texture : sorted_textures) {
			bx = ceil(texture->m_width / blockSize);	// round up
			by = ceil(texture->m_height / blockSize);

			// find a spot of the placement map
			px = py = 0;
			bool ok = false;

			for (auto i = 0; i < bsize * bsize; i++) {
				// test each position to find one we can start checking
				if (!placement_map[py * bsize + px]) {
					ok = true;	// suppose we have a spot

					// check availability on the Y axis
					for (auto y = py; y < py + by; y++) {
						// check availability on the X axis
						for (auto x = px; x < px + bx; x++) {
							if (placement_map[y * bsize + x]) {
								ok = false;	// actually, no we don't have a spot
								break;
							}
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
				allplaced = false;
				bsize++;
				delete[] placement_map;
				delete m_megatexture;

				break;
			}
			c++;

			// mark the object on the placement map
			for (auto y = py; y < py + by; y++) {
				for (auto x = px; x < px + bx; x++) {
					placement_map[y * bsize + x] = true;
				}
			}

			// copy the texture into the map
			int source_line = 0;
			int bytes = texture->m_width * rgba;				// number of bytes per line
			int dest_line = py * blockSize * size * rgba + px * blockSize * rgba;

			for (auto y = 0; y < texture->m_height; y++) {
				// copy one line
				memcpy(m_megatexture + dest_line, texture->m_data + source_line, bytes);
				source_line += bytes;
				dest_line += size * rgba;
			}

			// TODO point to the megatexture (use small epsilon to avoid texture bleeding)
			// https://gamedev.stackexchange.com/questions/46963/how-to-avoid-texture-bleeding-in-a-texture-atlas
			texture->m_xoffset = (((px + bx) * (float)blockSize)) / size;
			texture->m_yoffset = (((py + by) * (float)blockSize)) / size;

			texture->m_mega_width = -bx * (float)blockSize / size;
			texture->m_mega_height = -by * (float)blockSize / size;
		}
	} while (!allplaced);

	// delete old textures data
//	for (auto texture : sorted_textures) {
//		free(texture->m_data);
//		texture->m_data = nullptr;
//	}

	// create the fwTexture
	m_fwtextures = new fwTexture(m_megatexture, size, size, rgba);

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
void dfAtlasTexture::bindToMaterial(fwMaterialBasic* material)
{
	material->addDiffuseMap(m_fwtextures);
	material->addUniform(m_shader_idx);
}

/**
 * Save the texture as a PNG
 */
void dfAtlasTexture::save(std::string file)
{
	m_fwtextures->save(file);
}

dfAtlasTexture::~dfAtlasTexture()
{
	delete m_megatexture;
	delete m_fwtextures;
	delete m_shader_idx;
}
