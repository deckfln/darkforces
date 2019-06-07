#include "fwTexture.h"
#include "include/stb_image.h"


fwTexture::fwTexture()
{
}

fwTexture::fwTexture(const std::string file) :
	name(file)
{
	// load and generate the texture
	data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
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