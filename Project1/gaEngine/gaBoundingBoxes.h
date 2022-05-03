#pragma once

/**
 * Place holder for bounding boxes to be draw on screen
 **/

#include <vector>
#include <glm/vec3.hpp>

class fwAABBox;
class fwMesh;
class fwGeometry;
class fwMaterialBasic;
class fwScene;

class gaBoundingBoxes
{
	bool m_added2scene = false;
	std::vector<const fwAABBox*> m_boxes;
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_colors;

	bool m_dirty = false;

	fwMesh* m_mesh = nullptr;
	fwGeometry* m_geometry = nullptr;
	fwMaterialBasic* m_material = nullptr;

public:
	gaBoundingBoxes();
	void add(fwAABBox* box);
	void remove(fwAABBox* box);
	void draw(fwScene* scene);
	~gaBoundingBoxes();
};

extern gaBoundingBoxes g_gaBoundingBoxes;