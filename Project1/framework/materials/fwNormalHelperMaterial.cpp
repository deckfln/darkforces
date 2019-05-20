#include "fwNormalHelperMaterial.h"


fwNormalHelperMaterial::fwNormalHelperMaterial() :
	fwMaterial("shaders/normalHelper/vertex.glsl", "shaders/normalHelper/fragment.glsl", "shaders/normalHelper/geometry.glsl")
{
}

fwNormalHelperMaterial::~fwNormalHelperMaterial()
{
}
