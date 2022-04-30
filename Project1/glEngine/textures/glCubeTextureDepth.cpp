#include "glCubeTextureDepth.h"

glCubeTextureDepth::glCubeTextureDepth(int width, int height):
	glCubeTexture(width, height, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST)
{

}
