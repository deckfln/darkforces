#pragma once

#include <string>
#include <cstdio>
#include <glm/vec3.hpp>
#include <vector>

#include "../dfModel.h"
#include "../../framework/fwMesh.h"

class dfFileSystem;
class dfPalette;
class fwGeometry;
class fwMesh;
class fwScene;
class dfBitmap;
class dfAtlasTexture;

struct df3DOQuadIndex;

enum {
	DF_3DO_SHADING_NONE,
	DF_3DO_SHADING_GOURAUD,
	DF_3DO_SHADING_VERTEX
};

class df3DO : public dfModel
{
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_colors;
	std::vector<glm::vec3> m_textures;

	std::vector<dfBitmap*> m_textureNames;
	class dfAtlasTexture* m_atlasTexture = nullptr;

	fwGeometry* m_geometry = nullptr;
	fwMesh* m_mesh = nullptr;

	fwMeshRendering m_shading = fwMeshRendering::FW_MESH_TRIANGLES;
	glm::vec3 m_perVertexColor = glm::vec3(0);	// From DEATH.3DO it seems the first triagnle color is then applied to all vertices when rendering = 'vertex'

	void parseVertices(std::istringstream& infile, int nbVertices);
	void parseQuads(std::istringstream& infile, dfPalette *palette, int nbQuads);
	void parseTriangles(std::istringstream& infile, dfPalette* palette, int nbTriangles);
	void parseTextures(std::istringstream& infile, int nbVertives);
	void parseTexturesQuad(std::istringstream& infile, int nbQuads);
	void addVertice(df3DOQuadIndex* quad, int vertexIDX);
	void buildObject(void);

public:
	df3DO(dfFileSystem* fs, dfPalette* palette, std::string file);
	fwMesh* clone();
	~df3DO();
};