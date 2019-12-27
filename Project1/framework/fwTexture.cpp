#include "fwTexture.h"
#include "../include/stb_image.h"


fwTexture::fwTexture() :
	myclass(TEXTURE)
{
}

fwTexture::fwTexture(const std::string file, int nb_channels_to_read):
	myclass(TEXTURE),
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
	width(_width),
	height(_height),
	nrChannels(format)
{
}

fwTexture::fwTexture(unsigned char *data, int _width, int _height, int format) :
	myclass(TEXTURE),
	width(_width),
	height(_height),
	nrChannels(format),
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

fwTexture::~fwTexture()
{
	if (m_data != nullptr) {
		stbi_image_free(m_data);
	}
}