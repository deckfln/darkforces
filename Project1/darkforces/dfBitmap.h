#pragma once

#include <string>
#include <vector>

#include "dfFileGOB.h"

struct dfBitmapImage {
	int m_height = 0;
	int m_width = 0;
	int m_nrChannels = 0;
	bool m_transparent;
	int bsize = 0;

	float m_xoffset = 0;	// start x offset in the megatexture
	float m_yoffset = 0;	// start y offset in the megatexture
	float m_mega_width = 0;	// end x offset in the megatexture
	float m_mega_height = 0;	// end y offset in the megatexture

	long m_size = 0;
	char *m_data = nullptr;
};

class dfBitmap {
	void* m_data = nullptr;
	bool m_multiple = false;
	int m_nbImages = 1;
	int m_framerate = 0;
	std::vector<dfBitmapImage> m_images;

public:
	std::string m_name;
	dfBitmap(dfFileGOB* gob, std::string file);
	dfBitmapImage* getImage(int index = 0);
	~dfBitmap();
};