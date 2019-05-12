#pragma once
#include <iostream>

class Texture
{
	std::string name;
	unsigned char *data = nullptr;
	int width;
	int height;
	int nrChannels;

public:
	Texture();
	Texture(std::string file);
	Texture(int width, int height, int format);
	unsigned char *get_info(int *width, int *height, int *nrChannels);
	~Texture();
};

