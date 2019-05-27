#include "fwDepthMaterial.h"

fwDepthMaterial::fwDepthMaterial() :
	fwMaterial("shaders/depth/vertex.glsl", "shaders/depth/fragment.glsl", "")

{
}


fwDepthMaterial::~fwDepthMaterial()
{
};