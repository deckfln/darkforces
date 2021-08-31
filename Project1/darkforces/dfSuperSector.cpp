#include "dfSuperSector.h"

#include <algorithm>
#include <array>
#include <glm/glm.hpp> 
#include <imgui.h>

#include "../include/earcut.hpp"

#include "../framework/fwScene.h"
#include "../framework/geometries/fwGeometrySphere.h"

#include "../gaEngine/World.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"

#include "dfLevel.h"
#include "dfSign.h"
#include "dfMesh.h"

static glm::vec4 white(1.0, 0.0, 1.0, 1.0);
static fwMaterialBasic* material_portal = new fwMaterialBasic(&white);
static const char* g_className = "dfSuperSector";

dfSuperSector::dfSuperSector(dfSector* sector, fwMaterialBasic* material, std::vector<dfBitmap*>& bitmaps) :
	gaEntity(GameEngine::Entity::SECTOR),
	m_material(material)
{
	// link sector and supersector
	m_sectors.push_back(sector);
	sector->supersector(this);

	m_class_name = g_className;
	m_worldBounding = sector->m_worldAABB;
	m_name = sector->name();

	m_hasCollider = true;	// can collide
	m_physical = true;		// need to be tested for collision
	m_movable = false;		// cannot be pushed aside

	m_dfmesh = new dfMesh(material, bitmaps);
}

/**
 * return the surface of the sector bounding box
 */
float dfSuperSector::boundingBoxSurface(void)
{
	return m_worldBounding.surface();
}

/**
 * merge a super-sector into this one
 */
void dfSuperSector::extend(dfSuperSector* ssector)
{
	// extend the bounding box
	m_worldBounding.extend(ssector->m_worldBounding);

	// extend the list of sectors
	for (auto sector : ssector->m_sectors) {
		m_sectors.push_back(sector);
		sector->supersector(this);
	}

	m_name += ";" + ssector->m_name;

	// drop the portals between the 2 super sectors
	m_portals.remove_if([ssector](dfPortal& portal) { return portal.m_target == ssector;});
	ssector->m_portals.remove_if([this](dfPortal& portal) { return portal.m_target == this;});

	// parse all portals from the super-sector. 
	// It will be removed => adjoint super-sectors have to rebound to point to the new super-sector
	for (auto &portal : ssector->m_portals) {
		for (auto &remote_portal : portal.m_target->m_portals) {
			if (remote_portal.m_target == ssector) {
				remote_portal.m_target = this;
			}
		}
	}

	// extend the list of portals
	m_portals.insert(m_portals.end(), ssector->m_portals.begin(), ssector->m_portals.end());
}

/**
 * Extend the AABBox
 */
void dfSuperSector::extendAABB(fwAABBox& box)
{
	m_worldBounding.extend(box);
}

/**
 * if the segment collide with the sector
 */
float dfSuperSector::collide(const glm::vec3& start, const glm::vec3& end, fwCollision::Test test)
{
	return m_collider.collision(start, end, test);
}

/**
 * let an entity deal with a situation
 */
void dfSuperSector::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case gaMessage::Action::HIDE:
	case gaMessage::Action::UNHIDE:
		for (auto sector : m_sectors) {
			sector->sendInternalMessage(message->m_action);
		}
		break;
	}

	gaEntity::dispatchMessage(message);
}

/**
 * quick test to find AABB collision and return the collision point
 * test against the included dfSectors
 */
float dfSuperSector::collideAABBz(const fwAABBox& box)
{
	if (m_worldBounding.intersect(box)) {

		for (auto sector : m_sectors) {
			fwAABBox& worldAABB = sector->worldAABB();
			if (worldAABB.intersect(box)) {
				if (box.m_p.y > worldAABB.m_p.y) {
					// if box if over entity, return the bottom of box
					return worldAABB.m_p1.y;
				}

				// if box if below the entity, return the top of box
				return worldAABB.m_p.y;
			}
		}

	}
	return INFINITY;
}

/**
 * parse all portals to find the smalled adjacent
 */
dfSuperSector* dfSuperSector::smallestAdjoint(void)
{
	float surface, min_surface = 99999;
	dfSuperSector* smallest = nullptr;
	for (auto& portal : m_portals) {
		surface = portal.m_target->boundingBoxSurface();
		if (surface < min_surface) {
			min_surface = surface;
			smallest = portal.m_target;
		}
	}
	return smallest;
}

/**
 * Convert a position from level space to opengl space
 */
static glm::vec3 level2gl(glm::vec3 &levelSpace) {
	glm::vec3 a;

	a.x = levelSpace.x / (float)10.0;
	a.y = levelSpace.z / (float)10.0;
	a.z = levelSpace.y / (float)10.0;

	return a;
}

/**
 * parse the walls and build a list of portals in level space
 */
void dfSuperSector::buildPortals(std::vector<dfSector*>& sectors, std::vector<dfSuperSector*>& vssectors)
{
	dfSector* root = m_sectors.front();	// only one sector in the list

	for (auto wall : root->m_walls) {
		if (wall->m_adjoint >= 0) {
			// this wall is a portal
			dfSector* target = sectors[wall->m_adjoint];

			// portal size in level space
			float floor = std::max(root->staticFloorAltitude(), target->staticFloorAltitude());
			float ceiling = std::min(root->staticCeilingAltitude(), target->staticCeilingAltitude());
			glm::vec3 bottomLeft(
				root->m_vertices[wall->m_left].x,
				root->m_vertices[wall->m_left].y,
				floor);
			glm::vec3 bottomRight(
				root->m_vertices[wall->m_right].x,
				root->m_vertices[wall->m_right].y,
				floor);
			glm::vec3 topRight(
				root->m_vertices[wall->m_right].x,
				root->m_vertices[wall->m_right].y,
				ceiling);

			// and now to opengl
			topRight = level2gl(topRight);
			bottomLeft = level2gl(bottomLeft);
			bottomRight = level2gl(bottomRight);

			glm::vec3 normal = glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight));

			glm::vec3 center = topRight + bottomLeft;
			center /= 2.0;

			float dx = pow(topRight.x - bottomLeft.x, 2);
			float dy = pow(topRight.y - bottomLeft.y, 2);
			float dz = pow(topRight.z - bottomLeft.z, 2);
			float radius = sqrt(dx + dy + dz) / 2.0f;
			fwSphere boundingSphere = fwSphere(center, radius);

			m_portals.push_back(dfPortal(normal, boundingSphere, vssectors[wall->m_adjoint]));
		}
	}
}

/**
 * test if a super-sector contains a specific sector
 */
bool dfSuperSector::contains(int sectorID)
{
	for (auto sector : m_sectors) {
		if (sector->m_id == sectorID) {
			return true;
		}
	}
	return false;
}
/**
 * return the sector fitting the position
 */
dfSector* dfSuperSector::findDFSector(const glm::vec3& position)
{
	// position is in level space
	if (inAABBox(position)) {
		for (auto sector : m_sectors) {
			if (sector->isPointInside(position, false)) {
				return sector;
			}
		}
	}

	return nullptr;	// not here
}

/**
 * is the point precisely in that sector
 */
bool dfSuperSector::isPointIn(const glm::vec3& position)
{
	return findDFSector(position) != nullptr;
}

/**
 * Create the geometry
 */
void dfSuperSector::buildGeometry(std::vector<dfSector*>& sectors)
{
	for (auto sector : m_sectors) {
		sector->buildGeometry(m_dfmesh, dfWallFlag::NOT_MORPHS_WITH_ELEV);
	}

	m_dfmesh->name(m_name);
	m_dfmesh->buildMesh();

	// add the dfMesh as component to the entity
	GameEngine::ComponentMesh* mesh = new GameEngine::ComponentMesh(m_dfmesh);
	addComponent(mesh, gaEntity::Flag::DELETE_AT_EXIT);

	// override the mesh::geometry collider
	m_collider.set(
		(GameEngine::AABBoxTree*)&m_dfmesh->modelAABB(),
		m_dfmesh->pWorldMatrix(),
		m_dfmesh->pInverseWorldMatrix(),
		this, gaCollisionPoint::Source::SECTOR);


#ifdef _DEBUG
	if (m_debugPortals) {
		// add the portals on screen
		for (auto& portal : m_portals) {
			fwGeometrySphere* sphere = new fwGeometrySphere(portal.m_boundingSphere.radius());
			portal.m_debug_portal = new fwMesh(sphere, material_portal);
			glm::vec3 position = portal.m_boundingSphere.center();

			portal.m_debug_portal->translate(position);
		}
	}
#endif
}

/**
 * change the visibility of the super sector
 */
void dfSuperSector::visible(bool v)
{
	m_visible = v;
	m_dfmesh->set_visible(v);
}

/**
 * return the level list of textures
 */
std::vector<dfBitmap*>& dfSuperSector::textures(void)
{
	return m_parent->textures();
}

/**
 * return the level material
 */
fwMaterial* dfSuperSector::material(void)
{
	return m_parent->material();
}

/**
 * test all portals of the super-sector against the camera frustum.
 * for any visible portal, add the attached super-sector to the list of visible super-sectors
 * and test the portals again. Avoid coming back to already visited super-sectors
 */
void dfSuperSector::checkPortals(fwCamera* camera, int zOrder)
{
	m_dfmesh->zOrder(zOrder);

//	printf("dfSuperSector::checkPortals: %s\n", name().c_str());

	for (auto& portal : m_portals) {
		// ignore already activated portals
		if (portal.m_target->m_visible) {
			continue;
		}

//		printf(">dfSuperSector::checkPortals: %s\n", portal.m_target->name().c_str());

		// WARNING : the camera is using opengl space, but the boundSphere are translated to opengl space
		if (camera->is_inFrustum(portal.m_boundingSphere)) {

//				printf(">>dfSuperSector::checkPortals: in camera frustrum\n");

			// only if the portal is looking outward
			glm::vec3 look2portal = camera->get_position() - portal.m_boundingSphere.center();
			float l = glm::length(look2portal);
			look2portal = glm::normalize(look2portal);
			float d = glm::dot(look2portal, portal.m_normal);
			if (d >= 0 || l <= 1.0f) {
				// if the portal is near the camera, ignore the dot
				// there can be issues with the camera being slightly out of the current sector
//					printf(">>dfSuperSector::checkPortals: aligned player\n");

				dfSuperSector* target = portal.m_target;
				target->visible(true);
				target->checkPortals(camera, zOrder + 1);
			}
		}
	}
}

/**
 * create a full hierarchy
 */
void dfSuperSector::buildHiearchy(dfLevel* parent)
{
	m_parent = parent;
}

/**
 * parse the scene the find the super-sector and change its visibility
 * if the supers-sector is not on the scene and is visible => add to the scene
 */
void dfSuperSector::add2scene(fwScene* scene)
{
	m_dfmesh->display(scene, m_visible);
}

/**
 * Add a children mesh
 */
void dfSuperSector::addObject(dfMesh* object)
{
	m_dfmesh->addChild(object);
}

/**
 * Sort sectors by size. This is done for position testing. Sectors inside sectors will be tested first
 */
void dfSuperSector::sortSectors(void)
{
	m_sectors.sort([](dfSector* a, dfSector* b) { return a->boundingBoxSurface() < b->boundingBoxSurface(); });

	// take the opportunity to create a name for the super sector
	if (m_name.size() > 48) {
		m_name = m_name.substr(0, 48);
	}
}

/**
 * Update the ambient light in the dfMesh
 */
void dfSuperSector::updateAmbientLight(float ambient, int start, int len)
{
	m_dfmesh->changeAmbient(ambient, start, len);
}

/**
 * move the children sectors to mesh children
 */
void dfSuperSector::rebuildScene(fwScene* scene)
{
	// double check there is a mesh component here
	GameEngine::ComponentMesh* cmesh = static_cast<GameEngine::ComponentMesh *>(findComponent(gaComponent::MESH));
	if (!cmesh) {
		return;
	}
	
	fwMesh* ssmesh = cmesh->mesh();
	fwMesh* mesh = nullptr;

	// check each sub-sector to find a component mesh
	for (auto sector : m_sectors) {
		cmesh = static_cast<GameEngine::ComponentMesh*>(sector->findComponent(gaComponent::MESH));
		if (cmesh) {
			mesh = cmesh->mesh();
			ssmesh->addChild(mesh);
			scene->removeChild(mesh);
		}
	}
}

/**
 * Add dedicated component debug the entity
 */
void dfSuperSector::debugGUIChildClass(void)
{
	gaEntity::debugGUIChildClass();

	if (ImGui::TreeNode(g_className)) {
		ImGui::Checkbox("Visible", &m_visible);
		ImGui::Text("Ambient: %.02f", m_ambientLight);
		if (ImGui::TreeNode("Sectors")) {
			for (auto sector : m_sectors) {
				ImGui::Text(sector->name().c_str());
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}
dfSuperSector::~dfSuperSector()
{
	//delete m_mesh;

	for (auto sign : m_hSigns) {
		delete sign.second;
	}
}