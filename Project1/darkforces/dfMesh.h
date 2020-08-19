#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <array>

#include "../framework/fwMesh.h"
#include "../framework/fwScene.h"
#include "../framework/math/fwSphere.h"

#include "../gaEngine/gaComponent/gaComponentMesh.h"
#include "../gaEngine/AABBoxTree.h"

#include "dfBitmap.h"

const float dfOpengl2space = 10.0f;

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;
using Coord = float;
using Point = std::array<Coord, 2>;

class fwCylinder;

class gaCollisionPoint;

class dfSector;
class dfWall;
class dfSuperSector;
class dfVOC;

class dfMesh {
	std::vector<dfMesh*> m_children;

	// real buffers
	std::vector <glm::vec3> m_vertices;		// level vertices (based off m_position_lvl)
	std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
	std::vector <float> m_textureID;		// TextureID inside the megatexture
	std::vector <float> m_ambient;			// light intensity of the object

	std::vector<dfBitmap*> m_dummy;

	glm::vec3 m_position = glm::vec3(0);	// position of the mesh in GL space, by default (0,0,0)
	glm::vec3 m_defaultPosition = glm::vec3(0);// default position of the mesh in LEVEL space, (for translation off the position)

	fwMaterial* m_material = nullptr;
	fwGeometry* m_geometry = nullptr;
	GameEngine::ComponentMesh* m_mesh = nullptr;
	fwMesh* m_parentMesh = nullptr;

	bool m_visible = true;

	void setVertice(int p, float x, float y, float z, float xoffset, float yoffset, int textureID, float ambient);
	void updateRectangleAntiClockwise(int p, 
		glm::vec3& pStart,
		glm::vec3& pEnd,
		glm::vec2& offset,
		glm::vec2& size,
		int textureID,
		float ambient);

protected:
	std::string m_name;
	dfMesh* m_parent = nullptr;							// Parent dfMesh
	dfSuperSector* m_supersector = nullptr;
	GameEngine::AABBoxTree m_modelAABB;						// Model Space AABB
	std::vector<dfBitmap*>& m_bitmaps = m_dummy;		// image map

	// pointers, so we can reference other buffers
	std::vector<glm::vec3>* m_pVertices = nullptr;		// data of the sign in the GL buffer attributes
	std::vector<glm::vec2>* m_pUvs = nullptr;
	std::vector<float>* m_pTextureIDs = nullptr;
	std::vector <float>* m_pAmbientLights = nullptr;    // light intensity of the object

	void updateRectangle(int p, 
		glm::vec3& pStart,
		glm::vec3& pEnd,
		glm::vec2& offset,
		glm::vec2& size,
		int textureID,
		float ambient
	);
	int resize(int i);

public:
	dfMesh(fwMaterial* material, std::vector<dfBitmap*>& bitmaps);
	dfMesh(dfSuperSector*, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uvs, std::vector<float>* textureIDs, std::vector <float>* m_ambientLights, std::vector<dfBitmap*>& textures);
	dfMesh(dfMesh *parent);

	GameEngine::ComponentMesh* componentMesh(void) { return m_mesh; };
	fwMesh* mesh(void) { return m_mesh; };
	fwGeometry* geometry(void) { return m_geometry; };
	glm::mat4* worldMatrix(void) { return m_mesh->pWorldMatrix(); };
	glm::mat4* inverseWorldMatrix(void) { return m_mesh->pInverseWorldMatrix(); };

	const GameEngine::AABBoxTree& modelAABB(void);				// build the model AABB
	void addModelAABB(GameEngine::AABBoxTree* child);			// create a hierarchy of AABB pointing to triangles
	const glm::vec3& position(void) { return m_position; };

	void display(fwScene*, bool visibility);

	int addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, float ambient);
	void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, float ambient, bool clockwise);
	void addFloor(std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, float ambient, bool clockwise);
	void addPlane(float width, dfBitmapImage* image);
	virtual void buildGeometry(dfSector* source, float bottom, float top);

	bool collisionSegmentTriangle(const glm::vec3& p,
		const glm::vec3& q,
		std::list<gaCollisionPoint>& collisions,
		int firstVertex,
		int nbVertices);									// extended segment collision on limited set of triangles

	bool collisionSegmentTriangle(const glm::vec3& p,
		const glm::vec3& q,
		std::list<gaCollisionPoint>& collisions);			// extended segment collision test after a successful AABB collision

	bool checkCollision(fwCylinder& bounding,
		glm::vec3& direction,
		glm::vec3& intersection,
		std::string& name,
		std::list<gaCollisionPoint>& collisions);			// extended segment collision test after a successful AABB collision

	void parent(fwMesh* parent) { m_parentMesh = parent; };
	bool visible(void);
	void visible(bool status);
	void changeAmbient(float ambient, int start, int len);
	void addChild(dfMesh* mesh);
	void addMesh(fwMesh* mesh);
	void zOrder(int z);
	void updateGeometryTextures(int start, int nb);
	int nbVertices(void);									// current vertices on the geometry
	glm::vec3 const* vertice(uint32_t index);				// address of the vertices
	void name(std::string& name);

	void moveVertices(glm::vec3& center);
	void centerOnGeometryXZ(glm::vec3& target);
	void centerOnGeometryXYZ(glm::vec3& target);

	virtual void setStatus(int status) {};

	GameEngine::ComponentMesh* buildMesh(void);
	~dfMesh();
};