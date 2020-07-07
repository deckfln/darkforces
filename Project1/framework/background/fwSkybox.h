#pragma once
#include <string>

#include "../fwBackground.h"

class glCubeTexture;

class fwSkybox: public fwBackground
{
	glCubeTexture *m_texture = nullptr;

	void init(void);

public:
	fwSkybox(std::string *textures);
	fwSkybox(void* files[], int width, int height, int format);
	fwSkybox(void* data, int width, int height, int format);
	~fwSkybox();
};