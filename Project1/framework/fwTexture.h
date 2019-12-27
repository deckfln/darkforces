#pragma once
#include <iostream>

constexpr auto TEXTURE = 1;

class fwTexture
{
protected:
	int myclass = 0;
	const std::string name;
	unsigned char *m_data = nullptr;
	int width = 0;
	int height = 0;
	int nrChannels = 0;

public:
	fwTexture();
	fwTexture(const std::string file, int nb_channels_to_read = 0);
	fwTexture(int width, int height, int format);
	fwTexture(unsigned char *data, int width, int height, int format);

	unsigned char *get_info(int *width, int *height, int *nrChannels);
	bool is(const int mclass) { return myclass & mclass; };
	~fwTexture();
};

