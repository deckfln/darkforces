#include "fwGeometrySphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <list>

fwGeometrySphere::fwGeometrySphere(float radius, float widthSegments, float heightSegments)
{
    // copy/pasted from THREE.JS
	float phiStart = 0, phiLength = M_PI * 2, thetaStart = 0, thetaLength = M_PI;

    float thetaEnd = thetaStart + thetaLength;
    int index = 0;
    std::vector<std::vector<int>> grid;

    glm::vec3 vertex;
    glm::vec3 Vector3;

    // generate vertices, normals and uvs
    for (auto iy = 0; iy < heightSegments + 1; iy++) {
        std::vector<int> verticesRow;

        float v = iy / heightSegments;

        for (auto ix = 0; ix < widthSegments + 1; ix++) {
            float u = ix / widthSegments;

            // vertex
            vertex.x = -radius * cos(phiStart + u * phiLength) * sin(thetaStart + v * thetaLength);
            vertex.y = radius * cos(thetaStart + v * thetaLength);
            vertex.z = radius * sin(phiStart + u * phiLength) * sin(thetaStart + v * thetaLength);

            m_vertices.push_back(vertex);

            // normal
            m_normals.push_back(glm::normalize(vertex));

            // uv
            m_uvs.push_back(glm::vec2(u, 1 - v));

            verticesRow.push_back(index);
            index++;
        }
        grid.push_back(verticesRow);
    }

    // indices
    for (auto iy = 0; iy < heightSegments; iy++) {
        for (auto ix = 0; ix < widthSegments; ix++) {
            int a = grid[iy][ix + 1],
                b = grid[iy][ix],
                c = grid[iy + 1][ix],
                d = grid[iy + 1][ix + 1];

            if (iy != 0 or thetaStart > 0) {
                m_indices.push_back(a);
                m_indices.push_back(b);
                m_indices.push_back(d);
            }

            if (iy != heightSegments - 1 or thetaEnd < M_PI) {
                m_indices.push_back(b);
                m_indices.push_back(c);
                m_indices.push_back(d);
            }
        }
    }
    
    // build geometry
    addVertices("aPos", &m_vertices[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float));
    addAttribute("aTexCoord", GL_ARRAY_BUFFER, &m_uvs[0], 2, m_uvs.size() * sizeof(glm::vec2), sizeof(float));
    addAttribute("aNormal", GL_ARRAY_BUFFER, &m_normals[0], 3, m_normals.size() * sizeof(glm::vec3), sizeof(float));

    addIndex(&m_indices[0], 1, m_indices.size()*sizeof(unsigned int), sizeof(unsigned int));
}

fwGeometrySphere::~fwGeometrySphere()
{
}
