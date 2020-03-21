#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <string>

#include "dfMesh.h"

/**
 *  Store all data related to a sign
 */
class dfSuperSector;
class dfLogicTrigger;
class dfBitmap;
class dfSector;
class dfWall;
class dfMessage;

class dfSign : public dfMesh {
    int m_class = 0;
    dfBitmap *m_bitmap;                   // information on the bitmap overing the sign

    int m_start = 0;                    // index of the first attribute in the buffers
    int m_size = 0;                     // number of attributes

public:
    dfSign(dfSuperSector *ssector, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uvs, std::vector<float>* textureIDs, std::vector <float>* m_ambientLights, dfBitmap *bitmap, dfSector *sector, dfWall *wall, float z, float z1);
    void setClass(int clas) { m_class = clas; };
    void setStatus(int status);
    void rebuildAABB(void);
    void buildGeometry(dfSector* sector, dfWall* wall, float z, float z1);
    std::string& name(void) { return m_name; };
};