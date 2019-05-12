#include "Texture.h"
#include "include/stb_image.h"


Texture::Texture()
{
}

Texture::Texture(std::string file)
{
	// load and generate the texture
	name = file;
	data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
}

Texture::Texture(int _width, int _height, int format) :
	width(_width),
	height(_height),
	nrChannels(format)
{
}

unsigned char *Texture::get_info(int *_width, int *_height, int *_nrChannels)
{
	*_width = width;
	*_height = height;
	*_nrChannels = nrChannels;
	return data;
}

Texture::~Texture()
{
	if (data != nullptr) {
		stbi_image_free(data);
	}
}