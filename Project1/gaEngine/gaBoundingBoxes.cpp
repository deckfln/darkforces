#include "gaBoundingBoxes.h"

#include "../framework/fwAABBox.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwMaterialBasic.h"
#include "../framework/fwMesh.h"
#include "../framework/fwScene.h"

gaBoundingBoxes g_gaBoundingBoxes;

gaBoundingBoxes::gaBoundingBoxes()
{
}

/**
 * Add a boundingbox on the list
 */
void gaBoundingBoxes::add(const fwAABBox* box)
{
	m_boxes.push_back(box);
}

/**
 * Update the vertices for all AABB
 */
void gaBoundingBoxes::draw(fwScene* scene)
{
	// extend the storage if needed
	int sz = m_boxes.size() * 26;

	if (m_vertices.size() != sz) {
		m_vertices.resize(m_boxes.size() * 26);

		if (m_geometry != nullptr) {
			m_geometry->resizeAttribute("aPos", &m_vertices[0], m_vertices.size());
		}
	}

	// store the vertices
	int v = 0;
	bool dirty = false;

	for (unsigned i = 0; i < m_boxes.size(); i++) {

		if (((fwAABBox*)m_boxes[i])->updateMeshVertices(&m_vertices[v])) {
			dirty = true;
		}
		v += 26;
	}

	// build the mesh if it doesn't exists yet
	if (!m_added2scene) {
		m_added2scene = true;

		m_geometry = new fwGeometry();

		m_geometry->addVertices("aPos", &m_vertices[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float), false);

		// shared geometry
		static glm::vec4 w(1.0, 1.0, 1.0, 1.0);
		m_material = new fwMaterialBasic(&w);
		m_mesh = new fwMesh(m_geometry, m_material);
		m_mesh->rendering(fwMeshRendering::FW_MESH_LINES);
		m_mesh->always_draw();	// ignore frustum culling
		m_mesh->set_name("boundingboxes");

		scene->addChild(m_mesh);
	}
	else if (dirty) {
		m_geometry->update();
	}
}

gaBoundingBoxes::~gaBoundingBoxes()
{
	if (m_mesh) {
		delete m_material;
		delete m_geometry;
		delete m_mesh;
	}
}
