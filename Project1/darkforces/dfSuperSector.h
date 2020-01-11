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

class dfLevel;

class dfSuperSector {
    int m_id = -1;
    dfLevel* m_parent = nullptr;

    fwAABBox m_boundingBox;
    std::list <dfPortal> m_portals;
    std::list <dfSector*> m_sectors;
       
    std::vector <glm::vec3> m_vertices;		// level vertices
    std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
    std::vector <float> m_textureID;			// TextureID inside the megatexture
    std::map <int, glm::ivec3> m_sectorIndex;   // start of the sector vertices in the vertices buffer : x = start of walls, y = start of floors, z = #vertices on the floor. Ceiling vertices = y + z

    fwGeometry* m_geometry = nullptr;
    fwMesh* m_mesh = nullptr;               // static mesh of the super sector
                                            // moving meshes are children of the super sector (like elevators)

    bool m_visible = false;                 // supersector is visible on screen
    bool m_debugPortals = false;            // display the portal bounding sphere on screen

    void buildWalls(bool update, dfSector* sector, std::vector<dfTexture *>& textures, std::vector<dfSector*>& sectors);
    void buildFloor(bool update, dfSector* sector, std::vector<dfTexture*>& textures);
    void buildSigns(dfSector* sector, std::vector<dfTexture*>& textures, std::vector<dfSector*>& sectors);

    void updateRectangle(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, float textureID);
    int addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfTexture*>& textures);
    void addSign(dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfTexture*>& textures);

public:
    dfSuperSector(dfSector* sector);

    void extend(dfSuperSector*);
    dfSuperSector* smallestAdjoint(void);
    void buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*> &vssectors);
    float boundingBoxSurface(void);
    bool inAABBox(glm::vec3& position) { return m_boundingBox.inside(position); };
    dfSector* findSector(glm::vec3& position);
    bool contains(int sectorID);
    void buildGeometry(std::vector<dfSector*>& sectors, std::vector<dfTexture*>& m_textures, fwMaterialBasic* material);

    int id(void) { return m_id; };
    void visible(bool v) { m_visible = v; };
    bool visible(void) { return m_visible; };
    fwGeometry* geometry(void) { return m_geometry; };
    std::vector<dfTexture*>& textures(void);

    void checkPortals(fwCamera* camera, int zOrder);
    void parent(dfLevel* parent);
    void add2scene(fwScene* scene);
    void updateSectorVertices(int sectorID);
    fwMesh* buildElevator(dfSector* sector, float height, fwMaterial* material);
    void addRectangle(dfSector *sector, dfWall* wall, float z, float z1, glm::vec3& texture);

    void addObject(fwMesh* object);
    ~dfSuperSector();
};