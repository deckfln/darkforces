#pragma once
#include <iostream>

#include "../glad/glad.h"

constexpr auto TEXTURE = 1;

class fwTexture
{
protected:
	int m_id = 0;

	int m_class = 0;
	std::string m_name;
	unsigned char *m_data = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_nrChannels = 0;
	int m_filter = GL_LINEAR;

public:
	fwTexture();
	fwTexture(const std::string& file, int nb_channels_to_read = 0);
	fwTexture(int width, int height, int format);
	fwTexture(unsigned char *data, int width, int height, int format, int filter = GL_LINEAR);

	int filter(void) { return m_filter; };
	int id(void) { return m_id; };
	unsigned char *get_info(int *width, int *height, int *nrChannels);
	bool is(const int mclass) { return m_class & mclass; };
	bool save(const std::string& file);
	~fwTexture();
};

