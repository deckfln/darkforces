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
void gaBoundingBoxes::add(fwAABBox* box)
{
	bool f = false;
	int32_t lastEmpty = -1;

	for (size_t i = 0; i < m_boxes.size(); i++) {
		const fwAABBox* b = m_boxes[i];

		if (b == nullptr) {
			lastEmpty = i;
		}
		else if (b == box) {
			f = true;
			break;
		}
	}

	if (f == false) {
		if (lastEmpty < 0) {
			m_boxes.push_back(box);
		}
		else {
			m_boxes[lastEmpty] = box;
		}
		box->dirty();	// mark the AABB as dirty to refresh the vertices
		m_dirty = true;
	}
}

/**
 * Remove the mesh from the current scene
 */
void gaBoundingBoxes::remove(fwAABBox* box)
{
	for (unsigned i = 0; i < m_boxes.size(); i++) {
		if (m_boxes[i] == box) {
			m_boxes[i] = nullptr;

			// clear the vertices
			int k = i * 26;
			if (m_vertices.size() > k) {
				for (int j = 0; j < 26; j++) {
					m_vertices[k + j] = glm::vec3(0);
				}
			}

			m_dirty = true;
			break;
		}
	}
}

/**
 * Update the vertices for all AABB
 */
void gaBoundingBoxes::draw(fwScene* scene)
{
	// extend the storage if needed
	if (m_dirty) {
		int sz = m_boxes.size() * 26;
		if (m_vertices.size() < sz) {
			m_vertices.resize(sz);
			m_colors.resize(sz);

			if (m_geometry != nullptr) {
				m_geometry->resizeAttribute("aPos", &m_vertices[0], m_vertices.size());
				m_geometry->resizeAttribute("aColor", &m_colors[0], m_colors.size());
			}
		}
	}

	// store the vertices
	int v = 0;
	bool dirty = false;
	fwAABBox* box;

	for (unsigned i = 0; i < m_boxes.size(); i++) {
		box = (fwAABBox *)m_boxes[i];

		// update the vertices and the colors
		if (box && box->updateMeshVertices(&m_vertices[v], &m_colors[v])) {
			dirty = true;
		}
		v += 26;
	}

	// build the mesh if it doesn't exists yet
	if (m_boxes.size() > 0 && !m_added2scene) {
		m_added2scene = true;

		m_geometry = new fwGeometry();

		m_geometry->addVertices("aPos", &m_vertices[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float), false);
		m_geometry->addVertices("aColor", &m_colors[0], 3, m_colors.size() * sizeof(glm::vec3), sizeof(float), false);

		// shared geometry
		static glm::vec4 w(1.0, 1.0, 1.0, 1.0);
		m_material = new fwMaterialBasic(true);
		m_mesh = new fwMesh(m_geometry, m_material);
		m_mesh->rendering(fwMeshRendering::FW_MESH_LINES);
		m_mesh->always_draw(true);	// ignore frustum culling
		m_mesh->set_name("boundingboxes");

		scene->addChild(m_mesh);
	}
	else if (dirty || m_dirty) {
		m_dirty = false;
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
