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
    dfBitmap* m_bitmap = nullptr;

    int m_start = 0;                    // index of the first attribute in the buffers
    int m_size = 0;                     // number of attributes

public:
    dfSign(dfMesh* mesh, dfSector* sector, dfWall* wall, float z, float z1);
    void setClass(int clas) { m_class = clas; };
    void setStatus(int status);
    void rebuildAABB(void);
    void buildGeometry(dfSector* sector, dfWall* wall, float z, float z1);
    std::string& name(void) { return m_name; };
};