#include "fwTexture.h"
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

static int g_nbTextures = 0;

fwTexture::fwTexture() :
	m_class(TEXTURE),
	m_id(g_nbTextures++)
{
}

fwTexture::fwTexture(const std::string& file, int nb_channels_to_read):
	m_class(TEXTURE),
	m_id(g_nbTextures++),
	m_name(file)
{
	// load and generate the texture
	m_data = stbi_load(m_name.c_str(), &m_width, &m_height, &m_nrChannels, nb_channels_to_read);
	if (nb_channels_to_read > 0) {
		m_nrChannels = nb_channels_to_read;
	}
}

fwTexture::fwTexture(int _width, int _height, int format) :
	m_class(TEXTURE),
	m_id(g_nbTextures++),
	m_width(_width),
	m_height(_height),
	m_nrChannels(format)
{
}

fwTexture::fwTexture(unsigned char* data, int _width, int _height, int format, int filter) :
	m_class(TEXTURE),
	m_id(g_nbTextures++),
	m_width(_width),
	m_height(_height),
	m_nrChannels(format),
	m_filter(filter),
	m_data(data)
{
}

unsigned char *fwTexture::get_info(int *_width, int *_height, int *_nrChannels)
{
	*_width = m_width;
	*_height = m_height;
	*_nrChannels = m_nrChannels;
	return m_data;
}

/**
 * save the texture as PNG
 */
bool fwTexture::save(const std::string& file)
{
	return stbi_write_png(file.c_str(), m_width, m_height,m_nrChannels, m_data, m_width * m_nrChannels);
}

/**
 * set the bitmap to ZERO (or transparent if RGBA)
 */
void fwTexture::clear(void)
{
	if (m_data) {
		m_dirty = true;
		uint8_t* p = m_data;
		for (size_t i = 0; i < m_width * m_height * m_nrChannels; i++) {
			*(p++) = 0;
		}
	}
}

/**
 *
 */
void fwTexture::box(int32_t x, int32_t y, int32_t w, int32_t h, const glm::ivec4& color)
{
	if (m_data) {
		m_dirty = true;
		uint8_t* p = m_data;

		p = m_data + y * m_width * m_nrChannels + (m_width - x - 1) * m_nrChannels; //  += m_width * (m_height + y) * m_nrChannels - m_nrChannels * x;

		for (size_t ty = 0; ty < h; ty++) {
			for (size_t tx = 0; tx < w; tx++) {
				switch (m_nrChannels) {
				case 4:
					p[0] = color.r;
					p[1] = color.g;
					p[2] = color.b;
					p[3] = color.a;
					break;
				default:
					printf("fwTexture::box format not supported\n");
					__debugbreak();
				}
				p -= m_nrChannels;
			}
			p += m_width * m_nrChannels + m_nrChannels * w;
		}
	}
}

/**
 * Copy the bitmap into an atlasmap.
 * if the target size is bigger than the real size, place the bitmap at the defined corner
 */
void fwTexture::copyTo(uint8_t* target, uint32_t x, uint32_t y, uint32_t stride, uint32_t rgba, uint32_t Xcorner, uint32_t Ycorner)
{
	int source_line = 0;
	int bytes = m_width * m_nrChannels;				// number of real bytes per line
	int dest_line = (y + Ycorner) * stride * rgba + (x + Xcorner) * rgba;

	for (auto y = 0; y < m_height; y++) {
		// copy one line
		memcpy(target + dest_line, m_data + source_line, bytes);
		source_line += bytes;
		dest_line += stride * rgba;
	}
}

fwTexture::~fwTexture()
{
	if (m_data != nullptr) {
		stbi_image_free(m_data);
	}
}