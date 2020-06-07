#pragma once

#include <string>
#include <vector>

#include "dfPalette.h"

class dfFileSystem;
class fwSkybox;
class fwSkyline;

enum {
	DF_BITMAP_IMAGE_BOTTOM_CENTER
};

class dfBitmapImage {
public:
	int m_textureID = 0;		// offset of the texture in the TextureAtlas index
	float m_xoffset = 0;		// start x offset in the megatexture
	float m_yoffset = 0;		// start y offset in the megatexture
	float m_mega_width = 0;		// end x offset in the megatexture
	float m_mega_height = 0;	// end y offset in the megatexture

	int m_height = 0;			// real size of the bitmap
	int m_width = 0;
	int m_nrChannels = 0;		// depth of the bitmap
	bool m_transparent;
	int m_targetHeight = 0;		// target bitmap size (center the real bitmap inside)
	int m_targetWidth = 0;

	int m_bsize = 0;				// size in 4x4 blocks (based on the target size)
	int m_bsizeWidth = 0;		// target size in 4x4 blocks
	int m_bsizeHeight = 0;

	bool copied = false;		// texture is stored

	long m_size = 0;
	char *m_raw = nullptr;		// palette based raw data, stored by column  first
	char* m_data = nullptr;		// RGB converted image, stored by row first

	void targetSize(int x, int y);
	void boardSize(int blockSize);
	fwSkybox* convert2skybox(void);
	fwSkyline* convert2skyline(void);
	virtual void copyTo(unsigned char* target, int x, int y, int stride, int rgba, int Xcorner = 0, int Ycorner=0);
};

class dfBitmap {
	void* m_data = nullptr;
	bool m_multiple = false;
	int m_nbImages = 1;
	int m_framerate = 0;
	std::vector<dfBitmapImage> m_images;
	char *convert2rgba(dfBitmapImage* raw, dfPalette* palette);

public:
	std::string m_name;
	dfBitmap(dfFileSystem* fs, std::string file, dfPalette *palette);
	int nbImages(void) { return m_images.size(); };
	dfBitmapImage* getImage(unsigned int index = 0);
	~dfBitmap();
};