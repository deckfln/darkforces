#pragma once

#include <list>
#include <map>
#include <glm/mat4x4.hpp>

#include "../framework/fwGeometry.h"
#include "../framework/fwCamera.h"
#include "../framework/fwScene.h"
#include "../framework/fwMaterialBasic.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/Collider.h"
#include "../gaEngine/AABBoxTree.h"

#include "dfSector.h"
#include "dfBitmap.h"

class fwScene;
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

class dfLevel;
class dfMesh;

class dfSuperSector : public gaEntity
{
    bool m_removed = false;                     // yes we can delete the super sector
    dfLevel* m_parent = nullptr;

    std::list <dfPortal> m_portals;             // lits of portals driving to other SuperSectors
    std::list <dfSector*> m_sectors;            // list of basic sectors in the SuperSector
       
    std::vector <float> m_ambientLight;		    // % of ambient light
    std::map <int, glm::ivec3> m_sectorIndex;   // start of the sector vertices in the vertices buffer : x = start of walls, y = start of floors, z = #vertices on the floor. Ceiling vertices = y + z

    fwMaterialBasic* m_material = nullptr;
                                                // moving meshes are children of the super sector (like elevators)
    dfMesh* m_dfmesh = nullptr;

    bool m_visible = false;                     // super sector is visible on screen
    bool m_debugPortals = false;                // display the portal bounding sphere on screen

    std::map<std::string, dfSign*> m_hSigns;    // Hash of maps on the super sector

public:
    dfSuperSector(dfSector* sector, fwMaterialBasic* material, std::vector<dfBitmap*>& m_bitmaps);

    void extend(dfSuperSector*);
    void extendAABB(fwAABBox& box);
    dfSuperSector* smallestAdjoint(void);
    void buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*> &vssectors);
    float boundingBoxSurface(void);

    dfSector* findSector(const glm::vec3& position);        // return the level sector
    bool contains(int sectorID);
    void buildGeometry(std::vector<dfSector*>& sectors);

    // getter/setter
    void visible(bool v);
    inline bool visible(void) { return m_visible; };
    inline void remove(void) { m_removed = true; };
    inline bool removed(void) { return m_removed; };
    inline const std::string& name(void) { return m_name; };
    inline const GameEngine::Collider& collider(void) { return m_collider; };
    std::vector<dfBitmap*>& textures(void);
    fwMaterial* material(void);
    std::map<std::string, dfSign*>& hSigns(void) { return m_hSigns; };

    void checkPortals(fwCamera* camera, int zOrder);
    void buildHiearchy(dfLevel* parent);
    void add2scene(fwScene* scene);
    void addObject(dfMesh* object);
    void sortSectors(void);

    void updateAmbientLight(float ambient, int start, int len);
    void rebuildScene(fwScene *scene);                      // move the children sectors to mesh children

    float collide(const glm::vec3& start, 
        const glm::vec3& end, 
        fwCollision::Test test);                            // if the segment collide with the sector, return the Y position of the sector

    void dispatchMessage(gaMessage* message);               // let an entity deal with a situation

    // flight recorder & debugger
    void debugGUIChildClass(void) override;					// Add dedicated component debug the entity

    ~dfSuperSector();
};