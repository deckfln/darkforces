#include "NormalHelperMaterial.h"


NormalHelperMaterial::NormalHelperMaterial() :
	Material("shaders/normalHelper/vertex.glsl", "shaders/normalHelper/fragment.glsl", "shaders/normalHelper/geometry.glsl")
{
}

NormalHelperMaterial::~NormalHelperMaterial()
{
}
