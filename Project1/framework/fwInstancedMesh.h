#pragma once
#include "fwMesh.h"

#include "../glEngine/glInstancedBufferAttribute.h"

constexpr auto INSTANCED_MESH = 16384;

class fwInstancedMesh : public fwMesh
{
	glInstancedBufferAttribute *positions = nullptr;
	int max_instances = 0;
	int instances_to_draw = 0;
	bool dirty_instances = true;

public:
	fwInstancedMesh(fwGeometry *_geometry, fwMaterial *_material, int instances, glm::mat4 *_positions);
	void set_instances_2_draw(int);
	void update_position(int start = 0, int count = -1);
	void draw(glProgram *);
	~fwInstancedMesh();
};