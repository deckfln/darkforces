#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "framework/Geometry.h"
#include "framework/Material.h"
#include "glEngine/glProgram.h"
#include "glEngine/glVertexArray.h"

#include "Object3D.h"

constexpr auto MESH = 1;

class Mesh: public Object3D
{
	Geometry *geometry;
	Material *material;

	std::map<GLuint, glVertexArray *>vao;

	bool visible;
	bool outlined;

public:
	Mesh(Geometry *_geometry, Material *_material);
	std::string getMaterialHash(void);
	Material *get_material(void);

	Mesh &set_visible(bool _visible = true);
	bool is_visible(void);

	Mesh &outline(bool _outlined);
	bool is_outlined(void);

	void draw(glProgram *);
	~Mesh();
};