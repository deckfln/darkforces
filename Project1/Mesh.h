#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glVertexArray.h"
#include "framework/Geometry.h"
#include "framework/Material.h"
#include "glEngine/glProgram.h"
#include "Object3D.h"

class Mesh: public Object3D
{
	Geometry &geometry;
	Material *material;

	glVertexArray *vao;

public:
	Mesh(Geometry &_geometry, Material *_material);
	void set_uniforms(glProgram &program);
	glProgram &run(void);
	void draw(void);
	~Mesh();
};