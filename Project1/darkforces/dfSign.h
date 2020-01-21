#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

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

    std::vector<glm::vec3>& m_vertices;    // data of the sign in the GL buffer attributes
    std::vector<glm::vec2>& m_uvs;
    std::vector<float>& m_textureIDs;

    int m_start = 0;                    // index of the first attribute in the buffers
    int m_size = 0;                     // number of attributes

public:
    dfSign(dfSuperSector *ssector, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<float>& textureIDs, dfBitmap *bitmap, int start, int size);
    void setClass(int clas) { m_class = clas; };
    void setStatus(int status);
};