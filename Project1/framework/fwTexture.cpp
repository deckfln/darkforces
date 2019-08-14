#include "fwTexture.h"
#include "../include/stb_image.h"


fwTexture::fwTexture()
{
}

fwTexture::fwTexture(const std::string file, int nb_channels_to_read):
	name("c:/dev/project1/project1/"+file)
{
	// load and generate the texture
	data = stbi_load(name.c_str(), &width, &height, &nrChannels, nb_channels_to_read);
	if (nb_channels_to_read > 0) {
		nrChannels = nb_channels_to_read;
	}
}

fwTexture::fwTexture(int _width, int _height, int format) :
	width(_width),
	height(_height),
	nrChannels(format)
{
}

unsigned char *fwTexture::get_info(int *_width, int *_height, int *_nrChannels)
{
	*_width = width;
	*_height = height;
	*_nrChannels = nrChannels;
	return data;
}

fwTexture::~fwTexture()
{
	if (data != nullptr) {
		stbi_image_free(data);
	}
}