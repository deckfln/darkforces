#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glVertexArray.h"
#include "Geometry.h"
#include "Material.h"
#include "glEngine/glProgram.h"
#include "Object3D.h"

class Mesh: public Object3D
{
	glm::mat4 model;
	glm::vec3 position;

	Geometry geometry;
	Material material;

	glVertexArray vao;

public:
	Mesh(glProgram &program, Geometry &_geometry, Material &_material);
	void set_uniforms(glProgram &program);
	void draw(glProgram &program);
	~Mesh();
};