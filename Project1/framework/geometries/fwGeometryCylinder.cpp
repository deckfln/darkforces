#include "fwGeometryCylinder.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <list>

/// create a cylinder fwGeometry
/// 
/// radius:
/// height:
/// widthSegments:
/// heightSegments: if negative means created backward
/// <returns></returns>
fwGeometryCylinder::fwGeometryCylinder(float radius, float height, float widthSegments, float heightSegments)
{
    // copy/pasted from THREE.JS
    float thetaStart = 0.0f;
    float thetaLength = (float)M_PI * 2.0f;
    float theta, sinTheta, cosTheta;
    float halfHeight = height / 2.0f;
    int index = 0;

    std::vector<std::vector<int>> grid;

    // direction
    float s = (heightSegments > 0) ? 1.0f : -1.0f;
    heightSegments = abs(heightSegments);

    // generate vertices, normals and uvs

    for (auto iy = 0; iy < heightSegments + 1; iy++) {
        std::vector<int> verticesRow;

        float v = iy / heightSegments;

        for (auto ix = 0; ix < widthSegments + 1; ix++) {
            float u = ix / widthSegments;

            theta = u * thetaLength + thetaStart;

            sinTheta = sin(theta);
            cosTheta = cos(theta);

            // vertex
            m_vertices.push_back(
                glm::vec3(radius * sinTheta, radius * cosTheta , -v * s * height)
            );

            // normal
            m_normals.push_back(
                glm::normalize(glm::vec3(sinTheta, cosTheta, 0.0))
            );

            // uv
            m_uvs.push_back(glm::vec2(u, 1 - v));

            verticesRow.push_back(index);
            index++;
        }
        grid.push_back(verticesRow);
    }

    // indices
    for (auto ix = 0; ix < widthSegments; ix++) {
        for (auto iy = 0; iy < heightSegments; iy++) {
            int a = grid[iy][ix],
                b = grid[iy + 1][ix],
                c = grid[iy + 1][ix + 1],
                d = grid[iy][ix + 1];

            m_indices.push_back(a);
            m_indices.push_back(b);
            m_indices.push_back(d);

            m_indices.push_back(b);
            m_indices.push_back(c);
            m_indices.push_back(d);
        }
    }

    int p = m_vertices.size();

    m_vertices.push_back(glm::vec3(0.025, 0.025, 0.025));
    m_vertices.push_back(glm::vec3(0.025, 0.025, -0.025));
    m_vertices.push_back(glm::vec3(-0.025, 0.025, -0.025));
    m_vertices.push_back(glm::vec3(-0.025, 0.025, 0.025));
    m_vertices.push_back(glm::vec3(0.025, -0.025, 0.025));
    m_vertices.push_back(glm::vec3(0.025, -0.025, -0.025));
    m_vertices.push_back(glm::vec3(-0.025, -0.025, -0.025));
    m_vertices.push_back(glm::vec3(-0.025, -0.025, 0.025));

    m_indices.push_back(p);
    m_indices.push_back(p+2);
    m_indices.push_back(p+1);

    m_indices.push_back(p);
    m_indices.push_back(p + 3);
    m_indices.push_back(p + 2);

    m_indices.push_back(p + 4);
    m_indices.push_back(p + 5);
    m_indices.push_back(p + 6);

    m_indices.push_back(p + 4);
    m_indices.push_back(p + 6);
    m_indices.push_back(p + 7);

    // build geometry
    addVertices("aPos", &m_vertices[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float));
    addAttribute("aTexCoord", GL_ARRAY_BUFFER, &m_uvs[0], 2, m_uvs.size() * sizeof(glm::vec2), sizeof(float));
    addAttribute("aNormal", GL_ARRAY_BUFFER, &m_normals[0], 3, m_normals.size() * sizeof(glm::vec3), sizeof(float));

    addIndex(&m_indices[0], 1, m_indices.size()*sizeof(unsigned int), sizeof(unsigned int));
}

fwGeometryCylinder::~fwGeometryCylinder()
{
}
