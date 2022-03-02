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
	m_dirty = true;
}

fwTexture::~fwTexture()
{
	if (m_data != nullptr) {
		stbi_image_free(m_data);
	}
}