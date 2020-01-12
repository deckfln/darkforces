#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <array>

#include "../framework/fwMesh.h"

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;
using Coord = float;
using Point = std::array<Coord, 2>;

class dfSector;
class dfWall;
struct dfTexture;

class dfMesh {
	std::vector <glm::vec3> m_vertices;		// level vertices
	std::vector <glm::vec2> m_uvs;			// UVs inside the source texture
	std::vector <float> m_textureID;			// TextureID inside the megatexture

	fwMaterial* m_material = nullptr;
	fwGeometry* m_geometry = nullptr;
	fwMesh* m_mesh = nullptr;

	void updateRectangle(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, float textureID);
	void updateRectangleAntiClockwise(int p, float x, float y, float z, float x1, float y1, float z1, float xoffset, float yoffset, float width, float height, float textureID);

public:
	dfMesh(fwMaterial* material);

	fwMesh* mesh(void) { return m_mesh; };

	void buildGeometry(dfSector* source, float bottom, float top);
	int addRectangle(int start, dfSector* sector, dfWall* wall, float z, float z1, int texture, std::vector<dfTexture*>& textures);
	void addRectangle(dfSector* sector, dfWall* wall, float z, float z1, glm::vec3& texture, std::vector<dfTexture*>& textures, bool clockwise);
	void addFloor(std::vector<Point>& vertices, std::vector<std::vector<Point>>& polygons, float z, glm::vec3& texture, std::vector<dfTexture*>& textures, bool clockwise);
	void moveFloorTo(float z);
	void moveCeilingTo(float z);
	fwMesh* buildMesh(void);
	~dfMesh();
};