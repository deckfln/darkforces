#pragma once

#include <list>
#include <map>
#include <glm/mat4x4.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwCamera.h"
#include "../framework/fwScene.h"
#include "../framework/fwMaterialBasic.h"

#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/Collider.h"

#include "dfSector.h"
#include "dfBitmap.h"

class dfSuperSector;
class dfSign;
class fwCylinder;

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
    glm::vec3 m_normal;         // direction of the portal

    fwMesh* m_debug_portal = nullptr;

    dfSuperSector* m_target = nullptr;
    dfPortal(glm::vec3& normal, fwSphere& bounding, dfSuperSector* target) { m_id = nbPortals++; m_normal = normal;  m_boundingSphere = bounding; m_target = target; };
};


/**
 * collection of sectors : normally a big one an a lot of small adjacent ones
 *  used for: space partitioner and opengl draw calls
 *  super sectors have a list of portals to outside sectors.
 *     the portals can be tested against the camera frustum to detect visibility and draw the visible connecting sectors
 */
static int nbSuperSectors = 0;

class dfLevel;
class dfMesh;

class dfSuperSector 
{
    bool m_removed = false;                     // yes we can delete the super sector
    int m_id = -1;
    std::string m_name;
    dfLevel* m_parent = nullptr;

    fwAABBox m_worldAABB;                       // GL space world AABB

    std::list <dfPortal> m_portals;             // lits of portals driving to other SuperSectors
    std::list <dfSector*> m_sectors;            // list of basic sectors in the SuperSector
       
    std::vector <glm::vec3> m_vertices;		    // level vertices
    std::vector <glm::vec2> m_uvs;			    // UVs inside the source texture
    std::vector <float> m_textureID;		    // TextureID inside the mega-texture
    std::vector <float> m_ambientLight;		    // % of ambient light
    std::map <int, glm::ivec3> m_sectorIndex;   // start of the sector vertices in the vertices buffer : x = start of walls, y = start of floors, z = #vertices on the floor. Ceiling vertices = y + z

    fwGeometry* m_geometry = nullptr;
    fwMaterialBasic* m_material = nullptr;
    fwMesh* m_mesh = nullptr;                   // static mesh of the super sector
                                                // moving meshes are children of the super sector (like elevators)
    dfMesh* m_dfmesh = nullptr;

    bool m_visible = false;                     // super sector is visible on screen
    bool m_debugPortals = false;                // display the portal bounding sphere on screen

    std::map<std::string, dfSign*> m_hSigns;    // Hash of maps on the super sector

    glm::mat4 m_worldMatrix;
    glm::mat4 m_inverseWorldMatric;
    GameEngine::Collider m_collider;            // collider representing the dfSuperSector

public:
    dfSuperSector(dfSector* sector, fwMaterialBasic* material, std::vector<dfBitmap*>& m_bitmaps);

    void extend(dfSuperSector*);
    void extendAABB(fwAABBox& box);
    bool isPointInside(const glm::vec3& position);           // check if point is inside the AABB
    dfSuperSector* smallestAdjoint(void);
    void buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*> &vssectors);
    float boundingBoxSurface(void);

    dfSector* findSector(const glm::vec3& position);        // return the level sector
    bool contains(int sectorID);
    void buildGeometry(std::vector<dfSector*>& sectors);

    int id(void) { return m_id; };
    void visible(bool v) { m_visible = v; };
    bool visible(void) { return m_visible; };
    fwGeometry* geometry(void) { return m_geometry; };
    void remove(void) { m_removed = true; };
    bool removed(void) { return m_removed; };
    const std::string& name(void) { return m_name; };
    const GameEngine::Collider& collider(void) { return m_collider; };
    std::vector<dfBitmap*>& textures(void);
    std::map<std::string, dfSign*>& hSigns(void) { return m_hSigns; };

    void checkPortals(fwCamera* camera, int zOrder);
    void buildHiearchy(dfLevel* parent);
    void add2scene(fwScene* scene);
    void addObject(dfMesh* object);
    void sortSectors(void);

    void updateAmbientLight(float ambient, int start, int len);

    bool inAABBox(const glm::vec3& position) { 
        return m_worldAABB.inside(position); 
    };                                                  // test if vec3 inside the AABB
    bool collideAABB(const fwAABBox& box);				// quick test to find AABB collision
    bool collisionSegmentTriangle(const glm::vec3& p, 
        const glm::vec3& q, 
        std::list<gaCollisionPoint>& collisions);       // extended segment collision test after a successful AABB collision

    ~dfSuperSector();
};