#pragma once
#include <iostream>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

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
	bool m_dirty = false;		// data is dirty

public:
	fwTexture();
	fwTexture(const std::string& file, int nb_channels_to_read = 0);
	fwTexture(int width, int height, int format);
	fwTexture(unsigned char *data, int width, int height, int format, int filter = GL_LINEAR);

	int filter(void) { return m_filter; };
	int id(void) { return m_id; };
	unsigned char *get_info(int *width, int *height, int *nrChannels);
	bool save(const std::string& file);
	void clear(void);										// set the bitmap to ZERO (or transparent if RGBA)
	void box(int32_t x, int32_t y, int32_t w, int32_t h, const glm::ivec4& color);	// draw a box on the bitmap
	void copyTo(uint8_t *target, uint32_t x, uint32_t y, uint32_t stride, uint32_t rgba, uint32_t Xcorner=0, uint32_t Ycorner=0); //Copy the bitmap into an atlasmap.

	// getter/setter
	inline bool is(const int mclass) { return m_class & mclass; };
	inline void data(uint8_t* data) { m_data = data; };
	inline bool isDirty(void) { return m_dirty; };
	inline void dirty(bool b=true) { m_dirty = b; };
	inline const glm::ivec2 size(void) { return glm::ivec2(m_width, m_height); };


	~fwTexture();
};

