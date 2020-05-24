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

enum {
	DF_3DO_SHADING_NONE,
	DF_3DO_SHADING_GOURAUD,
	DF_3DO_SHADING_VERTEX
};

class df3DO : public dfModel
{
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_colors;

	std::vector<glm::vec3> m_verticesIndex;

	fwGeometry* m_geometry = nullptr;
	fwMesh* m_mesh = nullptr;

	fwMeshRendering m_shading = fwMeshRendering::FW_MESH_TRIANGLES;
	glm::vec3 m_perVertexColor = glm::vec3(0);	// From DEATH.3DO it seems the first triagnle color is then applied to all vertices when rendering = 'vertex'

	void parseVertices(std::istringstream& infile, int nbVertices);
	void parseQuads(std::istringstream& infile, dfPalette *palette, int nbQuads);
	void parseTriangles(std::istringstream& infile, dfPalette* palette, int nbTriangles);
	void addVertice(std::string& vertice, glm::vec3& color);
public:
	df3DO(dfFileSystem* fs, dfPalette* palette, std::string file);
	fwMesh* clone();
	~df3DO();
};