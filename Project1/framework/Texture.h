#pragma once
#include <iostream>

class Texture
{
	std::string name;
	unsigned char *data;
	int width;
	int height;
	int nrChannels;

public:
	Texture();
	Texture(std::string file);
	unsigned char *get_info(int *width, int *height, int *nrChannels);
	~Texture();
};

