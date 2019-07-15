#pragma once
#include <iostream>

class fwTexture
{
	const std::string name;
	unsigned char *data = nullptr;
	int width = 0;
	int height = 0;
	int nrChannels = 0;

public:
	fwTexture();
	fwTexture(const std::string file, int nb_channels_to_read = 0);
	fwTexture(int width, int height, int format);
	unsigned char *get_info(int *width, int *height, int *nrChannels);
	~fwTexture();
};

