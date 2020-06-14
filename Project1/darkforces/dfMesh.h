#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <array>

#include "../framework/fwAABBox.h"
#include "../framework/fwMesh.h"
#include "../framework/fwScene.h"
#include "../framework/math/fwSphere.h"

#include "dfBitmap.h"

const float dfOpengl2space = 10.0f;

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;
using Coord = float;
using Point = std::array<Coord, 2>;

enum fwCollisionPoint;
class fwCylinder;

class dfSector;
class dfWall;
class dfSuperSector;
class dfVOC;

class dfMesh {
	std::vector<dfMesh*> m_children;

	// real buffers
	std::vector <glm::vec3> m_vertices;		// level vertices (based off m_position)
	std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
	std::vector <float> m_textureID;		// TextureID inside the megatexture
	std::vector <float> m_ambient;			// light intensity of the object

	std::vector<dfBitmap*> m_dummy;

	glm::vec3 m_position = glm::vec3(0);	// position of the mesh in GL space, by default (0,0,0)
	glm::vec3 m_defaultPosition = glm::vec3(0);// default position of the mesh in LEVEL space, (for translation off the position)

	fwMaterial* m_material = nullptr;
	fwGeometry* m_geometry = nullptr;
	fwMesh* m_mesh = nullptr;
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
	void position(glm::vec3& position);
	void rotation(glm::vec3& rotation);
	void updateWorldBoundingBox(dfMesh *parent);

protected:
	std::string m_name;
	dfMesh* m_parent = nullptr;							// Parent dfMesh
	dfSuperSector* m_supersector = nullptr;
	fwAABBox m_boundingBox;								// Model Space AABB
	fwAABBox m_worldBoundingBox;						// Worl Space AABB
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

	fwMesh* mesh(void) { return m_mesh; };
	void display(fwScene*, bool visibility);

	int addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, float ambient);
	void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, float ambient, bool clockwise);
	void addFloor(std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, float ambient, bool clockwise);
	void addPlane(float width, dfBitmapImage* image);
	virtual void buildGeometry(dfSector* source, float bottom, float top);
	virtual void rebuildAABB(void);
	void moveFloorTo(float z);
	void moveCeilingTo(float z);
	void rotateZ(float angle);
	void move(glm::vec3& position);
	bool collide(float step, glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection, std::string& name);
	bool collide(fwCylinder& bounding, glm::vec3& target, glm::vec3& intersection, std::string& name, int& side);
	bool collide(fwAABBox& box, std::string& name);
	void parent(fwMesh* parent) { m_parentMesh = parent; };
	bool visible(void);
	void visible(bool status);
	void changeAmbient(float ambient, int start, int len);
	void addChild(dfMesh* mesh);
	void addMesh(fwMesh* mesh);
	void zOrder(int z);
	void updateGeometryTextures(int start, int nb);
	int nbVertices(void);
	void name(std::string& name);

	void moveVertices(glm::vec3& center);
	void centerOnGeometryXZ(glm::vec3& target);
	void centerOnGeometryXYZ(glm::vec3& target);

	virtual void setStatus(int status) {};

	fwMesh* buildMesh(void);

	// audio API
	bool play(dfVOC* voc=nullptr);
	void stop(dfVOC* voc=nullptr);
	~dfMesh();
};