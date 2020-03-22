#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <array>

#include "../framework/fwAABBox.h"
#include "../framework/fwMesh.h"
#include "../framework/math/fwSphere.h"

#include "dfBitmap.h"

const float dfOpengl2space = 10.0f;

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;
using Coord = float;
using Point = std::array<Coord, 2>;

class dfSector;
class dfWall;
class dfSuperSector;

class dfMesh {
	// real buffers
	std::vector <glm::vec3> m_vertices;		// level vertices (based off m_position)
	std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
	std::vector <float> m_textureID;		// TextureID inside the megatexture
	std::vector <float> m_ambient;			// light intensity of the object

	glm::vec3 m_position = glm::vec3(0);	// position of the mesh in GL space, by default (0,0,0)
	glm::vec3 m_defaultPosition = glm::vec3(0);// default position of the mesh in LEVEL space, (for translation off the position)

	fwMaterial* m_material = nullptr;
	fwGeometry* m_geometry = nullptr;
	fwMesh* m_mesh = nullptr;
	fwMesh* m_parent = nullptr;

	void setVertice(int p, float x, float y, float z, float xoffset, float yoffset, int textureID, float ambient);
	void updateRectangleAntiClockwise(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, int textureID, float ambient);

protected:
	std::string m_name;                    // sectorname(wallindex)
	dfSuperSector* m_supersector = nullptr;
	fwAABBox m_boundingBox;

	// pointers, so we can reference other buffers
	std::vector<glm::vec3>* m_pVertices = nullptr;    // data of the sign in the GL buffer attributes
	std::vector<glm::vec2>* m_pUvs = nullptr;
	std::vector<float>* m_pTextureIDs = nullptr;
	std::vector <float>* m_pAmbientLights = nullptr;    // light intensity of the object

	void updateRectangle(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, int textureID, float ambient);
	int resize(int i);

public:
	dfMesh(fwMaterial* material);
	dfMesh(dfSuperSector*, std::vector<glm::vec3>* vertices, std::vector<glm::vec2>* uvs, std::vector<float>* textureIDs, std::vector <float>* m_ambientLights);
	fwMesh* mesh(void) { return m_mesh; };

	int addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfBitmap*>& textures, float ambient);
	void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, std::vector<dfBitmap*>& textures, float ambient, bool clockwise);
	void addFloor(std::vector<Point>& vertices, std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, std::vector<dfBitmap*>& textures, float ambient, bool clockwise);
	void addPlane(float width, dfBitmapImage* image);
	virtual void buildGeometry(dfSector* source, float bottom, float top);
	virtual void rebuildAABB(void);
	void findCenter(void);
	void moveVertices(glm::vec3& center);
	void moveFloorTo(float z);
	void moveCeilingTo(float z);
	void rotateZ(float angle);
	void move(glm::vec3& position);
	void translate(glm::vec3& direction, float distance);
	bool collide(float step, glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection, std::string& name);
	bool collide(fwAABBox& box, std::string& name);
	void parent(fwMesh* parent) { m_parent = parent; };
	bool visible(void);
	void changeAmbient(float ambient);
	virtual void setStatus(int status) {};

	fwMesh* buildMesh(void);
	~dfMesh();
};