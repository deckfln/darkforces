#pragma once
#include <string>

#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"
#include "../glEngine/glCubeTexture.h"

#include "Camera.h"
#include "fwGeometry.h"
#include "Uniform.h"

class fwSkybox
{
	int id;
	glCubeTexture *texture = nullptr;
	glProgram *program = nullptr;
	glVertexArray *cube = nullptr;
	fwGeometry *geometry = nullptr;
	Uniform *uniform = nullptr;
	std::string get_shader(const std::string shader_file);

public:
	fwSkybox(std::string *textures);
	void draw(Camera *camera);
	~fwSkybox();
};