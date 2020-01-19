#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/**
 *  Store all data related to a sign
 */
class dfWall;
class dfLogicTrigger;

class dfSign {
    int m_class = 0;
    dfWall* m_parent = nullptr;
    int textureId = -1;

    glm::vec3* m_vertices = nullptr;    // data of the sign in the GL buffer attributes
    glm::vec2* m_uvs = nullptr;
    float* m_textureID = nullptr;

    int m_nbVertices = 0;

public:
    dfSign(dfWall* parent, glm::vec3* vertices, glm::vec2* uvs, float* textureID);
    void setClass(int clas) { m_class = clas; };
    void setStatus(int status);
    void nbVertice(int nb) { m_nbVertices = nb; };
};