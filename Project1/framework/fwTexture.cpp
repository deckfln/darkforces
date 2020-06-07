#include "fwTexture.h"
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

static int g_nbTextures = 0;

fwTexture::fwTexture() :
	myclass(TEXTURE),
	m_id(g_nbTextures++)
{
}

fwTexture::fwTexture(const std::string file, int nb_channels_to_read):
	myclass(TEXTURE),
	m_id(g_nbTextures++),
	name(file)
{
	// load and generate the texture
	m_data = stbi_load(name.c_str(), &width, &height, &nrChannels, nb_channels_to_read);
	if (nb_channels_to_read > 0) {
		nrChannels = nb_channels_to_read;
	}
}

fwTexture::fwTexture(int _width, int _height, int format) :
	myclass(TEXTURE),
	m_id(g_nbTextures++),
	width(_width),
	height(_height),
	nrChannels(format)
{
}

fwTexture::fwTexture(unsigned char* data, int _width, int _height, int format, int filter) :
	myclass(TEXTURE),
	m_id(g_nbTextures++),
	width(_width),
	height(_height),
	nrChannels(format),
	m_filter(filter),
	m_data(data)
{
}

unsigned char *fwTexture::get_info(int *_width, int *_height, int *_nrChannels)
{
	*_width = width;
	*_height = height;
	*_nrChannels = nrChannels;
	return m_data;
}

/**
 * save the texture as PNG
 */
bool fwTexture::save(std::string file)
{
	return stbi_write_png(file.c_str(), width, height, nrChannels, m_data, width * nrChannels);
}

fwTexture::~fwTexture()
{
	if (m_data != nullptr) {
		stbi_image_free(m_data);
	}
}