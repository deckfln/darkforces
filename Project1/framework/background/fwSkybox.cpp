#include "fwSkybox.h"

#include <string>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../fwCamera.h"
#include "../fwGeometry.h"
#include "../fwUniform.h"
#include "../../glEngine/glCubeTexture.h"

#include "../../glad/glad.h"
#include "../../include/stb_image.h"

const std::string cube_vs = "shaders/cubemap_vertex.glsl";
const std::string cube_fs = "shaders/cubemap_fragment.glsl";

float skyboxVertices[] = {
	// m_positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

/**
 *
 */
void fwSkybox::init(void)
{
	uniform = new fwUniform("skybox", texture);
	//TODO : the skybox textureID == 1 here
	uniform->set_uniform(program);

	geometry->addVertices("aPos", skyboxVertices, 3, sizeof(skyboxVertices), sizeof(float), false);

	geometry->enable_attributes(program);
	cube->unbind();
}

/**
 * Create a skypebox from a list of images
 */
fwSkybox::fwSkybox(std::string* textures):
	fwBackground(cube_vs, cube_fs)
{
	texture = new glCubeTexture(textures);
	init();
}

/**
 * Create a skypebox from a list of texture
 */
fwSkybox::fwSkybox(void* files[], int width, int height, int format) :
	fwBackground(cube_vs, cube_fs)
{
	texture = new glCubeTexture(files, width, height, format);
	init();
}

/**
 * Create a skypebox from a single texture
 */
fwSkybox::fwSkybox(void* data, int width, int height, int format) :
	fwBackground(cube_vs, cube_fs)
{
	texture = new glCubeTexture(data, width, height, format);
	init();
}

/**
 * Activate the uniforms for that background type
 */
void fwSkybox::setUniforms(glProgram*)
{
	program->set_uniform("skybox", texture);
}

fwSkybox::~fwSkybox()
{
	delete texture;
}
