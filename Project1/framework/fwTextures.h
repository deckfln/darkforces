#pragma once
#include <iostream>
#include <list>

#include "fwTexture.h"

constexpr auto TEXTURES = 2;

class fwTextures : fwTexture
{
	unsigned char** m_images = nullptr;	// IF there is a list of individual images
	int m_nbImages = 0;

public:
	fwTextures();
	fwTextures(const int nbImages, const std::string *files);
	fwTextures(std::list<fwTexture*> textures);
	fwTextures(const int nbImages, const int width, const int height, const int format);
	fwTextures(const int nbImages, const int width, const int height, const int format, unsigned char *raw_data);

	unsigned char *get_info(int &width, int &height, int &nrChannels, int &nrLayers);
	void set_image(int image, fwTexture* texture);

	~fwTextures();
};
