#pragma once

#include <list>
#include "../framework/fwAABBox.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwCamera.h"
#include "../framework/fwScene.h"
#include "../framework/fwMaterialBasic.h"

#include "dfSector.h"
#include "dfTexture.h"

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
    fwSphere m_boundingSphere;  // bounding sphere in opengl space

    fwMesh* m_debug_portal = nullptr;

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
       
    std::vector <glm::vec3> m_vertices;		// level vertices
    std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
    std::vector <UINT> m_textureID;			// TextureID inside the megatexture

    fwGeometry* m_geometry = nullptr;
    fwMesh* m_mesh = nullptr;

    bool m_visible = false;                 // supersector is visible on screen
    bool m_debugPortals = false;            // display the portal bounding sphere on screen

    void buildWalls(std::vector<dfTexture*>& textures, std::vector<dfSector*>& sectors);
    void buildFloor(std::vector<dfTexture*>& textures);
    void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfTexture*>& textures);

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
    void buildGeometry(std::vector<dfSector*>& sectors, std::vector<dfTexture*>& m_textures, fwMaterialBasic* material);
    fwGeometry* geometry(void) { return m_geometry; };
    void checkPortals(fwCamera* camera, int zOrder);
    void visible(bool v) { m_visible = v; };
    bool visible(void) { return m_visible; };
    void add2scene(fwScene* scene);
    ~dfSuperSector();
};