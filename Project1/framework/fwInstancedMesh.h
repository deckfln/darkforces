#pragma once
#include "fwMesh.h"

#include "../glEngine/glInstancedBufferAttribute.h"

constexpr auto INSTANCED_MESH = 16384;

class fwInstancedMesh : public fwMesh
{
	glInstancedBufferAttribute *positions = nullptr;
	int instances;

public:
	fwInstancedMesh(fwGeometry *_geometry, fwMaterial *_material, int instances, glm::mat4 *_positions);
	void draw(glProgram *);
	~fwInstancedMesh();
};