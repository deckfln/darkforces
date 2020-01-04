#pragma once

#include "../fwGeometry.h"
#include <vector>

class fwGeometrySphere : public fwGeometry
{
    // buffers
    std::vector<int> m_indices;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uvs;

public:
	fwGeometrySphere(float radius, float widthSegments = 8, float heightSegments = 6);
	~fwGeometrySphere();
};