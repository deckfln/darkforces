#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "fwGeometry.h"
#include "fwMaterial.h"
#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"

#include "fwObject3D.h"

constexpr auto MESH = 1;

class fwMesh: public fwObject3D
{
	bool visible = true;
	bool outlined = false;
	bool normalHelper = false;
	bool m_transparent = false;
	void *m_pExtra = nullptr;

protected:
	fwGeometry *geometry = nullptr;
	fwMaterial *material = nullptr;
	std::map<GLuint, glVertexArray *>vao;
	bool wireFrame = false;

public:
	fwMesh();
	fwMesh(fwGeometry *_geometry, fwMaterial *_material);

	std::string getMaterialHash(void);
	fwMaterial *get_material(void);
	fwGeometry *get_geometry(void);

	fwMesh &set_visible(bool _visible = true);
	bool is_visible(void);

	fwMesh &outline(bool _outlined);
	bool is_outlined(void);

	fwMesh &transparent(bool _transparent) { m_transparent = _transparent; return *this; };
	bool is_transparent(void) { return m_transparent; };

	fwMesh &extra(void *data) { m_pExtra = data; return *this;  }
	void *extra(void) { return m_pExtra; }

	fwMesh &show_normalHelper(bool _helper);
	bool is_normalHelper(void);

	fwMesh &draw_wireframe(bool _wireframe);

	virtual void draw(glProgram *);
	~fwMesh();
};