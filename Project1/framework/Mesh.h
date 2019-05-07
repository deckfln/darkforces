#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "framework/Geometry.h"
#include "framework/Material.h"
#include "glEngine/glProgram.h"
#include "glEngine/glVertexArray.h"

#include "Object3D.h"

class Mesh: public Object3D
{
	Geometry &geometry;
	Material *material;

	glVertexArray *vao;

public:
	Mesh(Geometry &_geometry, Material *_material);
	std::string getMaterialHash(void);
	Material *get_material(void);
	void set_uniforms(glProgram *program);
	void draw(glProgram *);
	~Mesh();
};