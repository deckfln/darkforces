#include "fwMaterialDepth.h"

fwMaterialDepth::fwMaterialDepth() :
	fwMaterial("shaders/depth/vertex.glsl", "shaders/depth/fragment.glsl", "")

{
}


fwMaterialDepth::~fwMaterialDepth()
{
};