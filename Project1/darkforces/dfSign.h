#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/**
 *  Store all data related to a sign
 */
class dfSuperSector;
class dfLogicTrigger;
class dfBitmap;

class dfSign {
    int m_class = 0;
    dfSuperSector* m_supersector;
    dfBitmap *m_bitmap;                   // information on the bitmap overing the sign

    glm::vec3* m_vertices = nullptr;    // data of the sign in the GL buffer attributes
    glm::vec2* m_uvs = nullptr;
    float* m_textureIDs = nullptr;

    int m_start = 0;                    // index of the first attribute in the buffers
    int m_size = 0;                     // number of attributes

public:
    dfSign(dfSuperSector *ssector, glm::vec3* vertices, glm::vec2* uvs, float* textureIDs, dfBitmap *bitmap, int start, int size);
    void setClass(int clas) { m_class = clas; };
    void setStatus(int status);
};