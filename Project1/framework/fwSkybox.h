#pragma once
#include <string>

#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"
#include "../glEngine/glCubeTexture.h"

#include "fwCamera.h"
#include "fwGeometry.h"
#include "fwUniform.h"

class fwSkybox
{
	int id;
	glCubeTexture *texture = nullptr;
	glProgram *program = nullptr;
	glVertexArray *cube = nullptr;
	fwGeometry *geometry = nullptr;
	fwUniform *uniform = nullptr;
	std::string get_shader(const std::string shader_file);

	void init(void);

public:
	fwSkybox(std::string *textures);
	fwSkybox(void* files[], int width, int height, int format);
	fwSkybox(void* data, int width, int height, int format);
	void draw(fwCamera *camera, int renderMode);
	~fwSkybox();
};