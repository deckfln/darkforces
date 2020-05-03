#pragma once

#include <string>
#include <vector>

#include "dfPalette.h"

class dfFileSystem;

class dfBitmapImage {
public:
	int m_textureID = 0;		// offset of the texture in the TextureAtlas index
	float m_xoffset = 0;		// start x offset in the megatexture
	float m_yoffset = 0;		// start y offset in the megatexture
	float m_mega_width = 0;		// end x offset in the megatexture
	float m_mega_height = 0;	// end y offset in the megatexture

	int m_height = 0;
	int m_width = 0;
	int m_nrChannels = 0;
	bool m_transparent;
	int bsize = 0;

	long m_size = 0;
	char *m_raw = nullptr;		// palette based raw data, stored by column  first
	char* m_data = nullptr;		// RGB converted image, stored by row first
};

class dfBitmap {
	void* m_data = nullptr;
	bool m_multiple = false;
	int m_nbImages = 1;
	int m_framerate = 0;
	std::vector<dfBitmapImage> m_images;
	char *convert2rgb(dfBitmapImage* raw, dfPalette* palette);

public:
	std::string m_name;
	dfBitmap(dfFileSystem* fs, std::string file, dfPalette *palette);
	int nbImages(void) { return m_images.size(); };
	dfBitmapImage* getImage(unsigned int index = 0);
	~dfBitmap();
};