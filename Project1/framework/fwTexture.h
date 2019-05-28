#pragma once
#include <iostream>

class fwTexture
{
	std::string name;
	unsigned char *data = nullptr;
	int width;
	int height;
	int nrChannels;

public:
	fwTexture();
	fwTexture(std::string file);
	fwTexture(int width, int height, int format);
	unsigned char *get_info(int *width, int *height, int *nrChannels);
	~fwTexture();
};

