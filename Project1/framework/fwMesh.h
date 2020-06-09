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

enum class fwMeshRendering {
	FW_MESH_TRIANGLES,
	FW_MESH_POINT,
	FW_MESH_LINE
};

class fwMesh: public fwObject3D
{
	bool visible = true;		// object is displayed in the scene
	bool m_always_draw = false;	// ignore frustrum visibility and always draw
	bool outlined = false;
	bool normalHelper = false;
	bool m_transparent = false;
	int m_zorder = 0;			// 0 => sort meshed by distance to the camera
								// 1... => the app is sorting the meshes. 1 is drawn first, then 2 ....
	void *m_pExtra = nullptr;
	std::list <fwUniform*> m_uniforms;	// meshes can have dedicated uniforms (not included in the material)
	fwMeshRendering m_rendering = fwMeshRendering::FW_MESH_TRIANGLES;	// Rendering mode of the mesh
	float m_pointSize = 1.0;			// for FW_MESH_POINT rendering

protected:
	fwGeometry *geometry = nullptr;
	fwMaterial *material = nullptr;
	std::map<GLuint, glVertexArray *>vao;
	bool wireFrame = false;
	GLuint buildVAO(glProgram* program);

public:
	fwMesh();
	fwMesh(fwGeometry *_geometry, fwMaterial *_material);

	fwMesh* clone(void);

	fwMesh& addUniform(fwUniform* uniform);
	void set_uniforms(glProgram* program);

	std::string getMaterialHash(void);
	fwMaterial *get_material(void);
	fwGeometry *get_geometry(void);

	fwMesh &set_visible(bool _visible = true);
	bool is_visible(void);

	fwMesh &outline(bool _outlined);
	bool is_outlined(void);

	fwMesh& always_draw(bool al) { m_always_draw = al; return *this; };
	bool always_draw(void) { return m_always_draw; };

	fwMesh &transparent(bool _transparent) { m_transparent = _transparent; return *this; };
	bool is_transparent(void) { return m_transparent; };

	fwMesh &extra(void *data) { m_pExtra = data; return *this;  }
	void *extra(void) { return m_pExtra; }

	fwMesh &show_normalHelper(bool _helper);
	bool is_normalHelper(void);

	fwMesh &draw_wireframe(bool _wireframe);

	int zOrder(void) { return m_zorder; };
	void zOrder(int z) { m_zorder = z; };

	void updateVertices(int offset = 0, int size = -1);
	void updateAttribute(const std::string &attribute, int offset = 0, int size = -1);

	float sqDistance2boundingSphere(glm::vec3 position);

	void rendering(fwMeshRendering render);
	void pointSize(float p) { m_pointSize = p; };

	void centerOnGeometry(void);

	virtual void draw(glProgram *);
	~fwMesh();
};