#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "fwGeometry.h"
#include "fwMaterial.h"
#include "fwAABBox.h"

#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"

#include "fwObject3D.h"

enum class fwMeshRendering {
	FW_MESH_TRIANGLES,
	FW_MESH_POINT,
	FW_MESH_LINE,
	FW_MESH_LINES
};

class fwMesh: public fwObject3D
{
	int m_id = 0;

	bool m_always_draw = false;				// ignore frustum visibility and always draw
	bool outlined = false;
	bool normalHelper = false;
	bool m_transparent = false;
	int m_zorder = 0;						// 0 => sort meshed by distance to the camera
											// 1... => the app is sorting the meshes. 1 is drawn first, then 2 ....
	void *m_pExtra = nullptr;
	std::list <fwUniform*> m_uniforms;		// meshes can have dedicated uniforms (not included in the material)

protected:
	fwMeshRendering m_rendering = fwMeshRendering::FW_MESH_TRIANGLES;	// Rendering mode of the mesh
	float m_pointSize = 1.0;				// for FW_MESH_POINT rendering

	fwGeometry *m_geometry = nullptr;
	fwMaterial *m_material = nullptr;
	std::map<GLuint, glVertexArray *>m_vao;
	GLuint buildVAO(glProgram* program);

public:
	fwMesh();
	fwMesh(fwGeometry *_geometry, fwMaterial *_material);
	fwMesh(fwMaterial* _material);

	inline int id(void) { return m_id; };
	fwMesh* clone(void);					// create a new clone
	void clone(fwMesh* source);				// shallow clone by assigning data to the target
	void set(fwGeometry* geometry, 
		fwMaterial* material);				// initialize a mesh

	fwMesh& addUniform(fwUniform* uniform);
	void set_uniforms(glProgram* program);

	std::string getMaterialHash(void);
	fwMaterial *get_material(void);
	fwGeometry *get_geometry(void);

	fwMesh &outline(bool _outlined);
	bool is_outlined(void);

	inline fwMesh& always_draw(bool al) { m_always_draw = al; return *this; };
	inline bool always_draw(void) { return m_always_draw; };
	inline fwMesh &transparent(bool _transparent) { m_transparent = _transparent; return *this; };
	inline bool is_transparent(void) { return m_transparent; };
	inline fwMesh &extra(void *data) { m_pExtra = data; return *this;  }
	inline void *extra(void) { return m_pExtra; }

	fwMesh &show_normalHelper(bool _helper);
	bool is_normalHelper(void);

	inline int zOrder(void) { return m_zorder; };
	inline void zOrder(int z) { m_zorder = z; };

	void updateVertices(int offset = 0, int size = -1);
	void updateAttribute(const std::string &attribute, int offset = 0, int size = -1);

	float sqDistance2boundingSphere(glm::vec3 position);

	void rendering(fwMeshRendering render);
	inline void pointSize(float p) { m_pointSize = p; };

	void centerOnGeometry(void);

	const fwAABBox& modelAABB(void);

	virtual void draw(glProgram *);

	void debugGUIChildClass(void) override;

	~fwMesh();
};