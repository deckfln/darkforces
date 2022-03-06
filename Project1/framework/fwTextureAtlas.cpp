#include "fwTextureAtlas.h"
#include <list>

#include "fwTexture.h"
#include "fwUniform.h"
#include "fwMaterialBasic.h"

/**
 * create
 */
Framework::TextureAtlas::TextureAtlas()
{
}

/**
 * add a fwTexture to the atlas
 */
void Framework::TextureAtlas::add(fwTexture* texture)
{
	m_source.push_back(texture);
}

/**
 * basic algorithm : use a square placement map, find an empty spot, store the texture. if no spot can be found, increase the texture size
 */
fwTexture* Framework::TextureAtlas::generate(void)
{
	struct Image {
		uint32_t index;			// index of the image on the list of images
		uint32_t pWidth;		// size in pixels
		uint32_t pHeight;
		uint32_t bsizeWidth;	// size in blocks
		uint32_t bsizeHeight;
		uint32_t bsize;			//
		float xoffset;			// position in the megatexture in %
		float yoffset;
		float width;			// size in the megatexture in %
		float height;
		fwTexture* texture;
	};

	std::list<Image*> sorted_images;
	const int blockSize = 4;
	int rgba=3;

	// count number of 16x16 blocks
	int blocks4x4 = 0;
	int bx, by;
	int32_t h, w;

	std::vector<Image> images;
	images.resize(m_source.size());

	size_t i = 0;
	uint32_t bsize;

	for (auto texture : m_source) {
		if (texture) {
			texture->get_info(&w, &h, &rgba);

			// conver size of the texture to the mminimum block size
			images[i].index = i;
			images[i].pWidth = w;
			images[i].pHeight = h;
			images[i].bsizeWidth = (uint32_t)ceil((float)w / (float)blockSize);
			images[i].bsizeHeight = (uint32_t)ceil((float)h / (float)blockSize);
			images[i].bsize = images[i].bsizeWidth * images[i].bsizeHeight;
			images[i].texture = texture;

			blocks4x4 += images[i].bsize;

			sorted_images.push_back(&images[i]);

			i++;
		}
	}

	// sort textures by size : big first
	sorted_images.sort([](Image* a, Image* b) { return a->bsize > b->bsize; });

	// evaluate the size of the megatexture (square texture of 4x4 blocks)
	bsize = (int)ceil(sqrt(blocks4x4));

	// try to place all objects on the map.
	// if fail, increase the size of the map until we place all textures

	bool allplaced = false;
	bool *placement_map;
	int size;

	// register the position of the texture on the atlas texture
	std::vector<glm::ivec2> texture_position;
	texture_position.resize(sorted_images.size());

	// place the texture on a 4x4 board
	do {
		// avaibility map of 16x16 blocks : false = available, true = used
		placement_map = new bool[bsize * bsize]();

		// find an available space on the map
		uint32_t px, py;
		int c = 0;
		glm::ivec2 first_available_position = glm::ivec2(0);
		int first_available_byte = 0;

		allplaced = true;	// suppose we will be able to fit all textures

		int iTex = 0;
		for (auto image : sorted_images) {
			bx = image->bsizeWidth;
			by = image->bsizeHeight;

			// find a spot of the placement map
			px = first_available_position.x;
			py = first_available_position.y;

			first_available_byte = first_available_position.x + first_available_position.y * bsize;
			bool ok = false;

			for (size_t i = 0; i < bsize * bsize; i++) {
				// test each position to find one we can start checking
				if (!placement_map[py * bsize + px]) {
					first_available_position.x = px;
					first_available_position.y = py;
					ok = true;	// suppose we have a spot

					for (size_t y = py; y < py + by; y++) {
						if (y >= bsize) {
							ok = false;
							break;
						}
						// check availability on the X axis
						for (size_t x = px; x < px + bx; x++) {
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
	m_megatexture.resize(size * size * rgba);

	int iTex = 0;
	int x, y;
	for (auto image: sorted_images) {
		// extract the position on the 4x4 board
		x = texture_position[iTex].x * blockSize;
		y = texture_position[iTex].y * blockSize;

		image->texture->copyTo(&m_megatexture[0], x, y, size, rgba);

		// TODO point to the megatexture (use small epsilon to avoid texture bleeding)
		// https://gamedev.stackexchange.com/questions/46963/how-to-avoid-texture-bleeding-in-a-texture-atlas
		image->xoffset = (float)(x + image->pWidth) / size;
		image->yoffset = (float)(y + image->pHeight) / size;

		image->width = -(float)image->pWidth / size;
		image->height = -(float)image->pHeight / size;

		iTex++;
	}

	// create the fwTexture
	m_fwtextures = new fwTexture(&m_megatexture[0], size, size, rgba, GL_NEAREST);

	// build the index
	m_megatexture_idx.resize(sorted_images.size());

	i = 0;
	for (auto image : sorted_images) {
		m_megatexture_idx[image->index] = glm::vec4(image->xoffset, image->yoffset, image->width, image->height);
		//image->m_textureID = i;
		i++;
	}
	m_shader_idx = new fwUniform("megatexture_idx", &m_megatexture_idx[0], i);

	return m_fwtextures;
}

/**
 * Bind the atlas texture to a material
 */
void Framework::TextureAtlas::bindToMaterial(fwMaterial* material, std::string uniform)
{
	if (material->type(BASIC_MATERIAL)) {
		((fwMaterialBasic* )material)->addDiffuseMap(m_fwtextures);
	}
	else {
		material->addTexture(uniform, m_fwtextures);
	}
	material->addUniform(m_shader_idx);
}

/**
 * Save the texture as a PNG
 */
void Framework::TextureAtlas::save(const std::string& file)
{
	m_fwtextures->save(file);
}

/**
 *
 */
fwTexture* Framework::TextureAtlas::texture(void)
{
	return m_fwtextures;
}

/**
 * extract the texture coordinates of the image at index 
 */
void Framework::TextureAtlas::texel(uint32_t index, glm::vec4& texel)
{
	texel = m_megatexture_idx[index];
}

/**
 *
 */
Framework::TextureAtlas::~TextureAtlas()
{
	delete m_fwtextures;
	delete m_shader_idx;
}
