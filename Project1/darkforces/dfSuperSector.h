#pragma once

#include <list>
#include "../framework/fwAABBox.h"

#include "dfSector.h"

class dfSuperSector;

/**
 * boundingSphere of a portal to a connected sector
 * can be tested against camera frustum to check if the connected sector is likely visible
 * back in '95 they needed to use a portal by sector
 * now we can use portals only for super sectors
 */
static int nbPortals=0;

class dfPortal {
public:
    int m_id = -1;
    fwSphere m_boundingSphere;

    dfSuperSector* m_target = nullptr;
    dfPortal(fwSphere& bounding, dfSuperSector* target) { m_id = nbPortals++; m_boundingSphere = bounding; m_target = target; };
};


/**
 * collection of sectors : normally a big one an a lot of small adjacent ones
 *  used for: space partitionnnig and opengl drawcalls
 *  supersectors have a list of portals to outside sectors.
 *     the portals can be tested against the camera frustum to detect visibility and draw the visible connecting sectors
 */
static int nbSuperSectors = 0;

class dfSuperSector {
    int m_id = -1;
    fwAABBox m_boundingBox;
    std::list <dfPortal> m_portals;
    std::list <dfSector*> m_sectors;
public:
    dfSuperSector(dfSector* sector);
    void extend(dfSuperSector*);
    dfSuperSector* smallestAdjoint(void);
    void buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*> &vssectors);
    float boundingBoxSurface(void);
    bool inAABBox(glm::vec3& position) { return m_boundingBox.inside(position); };
    dfSector* findSector(glm::vec3& position);
    bool contains(int sectorID);
    int id(void) { return m_id; };
};