#pragma once

#include "../fwGeometry.h"
#include <vector>

class fwGeometryCylinder : public fwGeometry
{
    // buffers
    std::vector<int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;

public:
    fwGeometryCylinder(float radius, float height, float widthSegments = 8, float heightSegments = 1);
	~fwGeometryCylinder();
};