#pragma once

#include <string>
#include <cstdio>
#include <glm/vec3.hpp>
#include <vector>

#include "../dfModel.h"

class dfFileSystem;
class dfPalette;
class fwGeometry;
class fwMesh;
class fwScene;

class df3DO : public dfModel
{
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec3> m_colors;

	std::vector<glm::vec3> m_verticesIndex;

	fwGeometry* m_geometry = nullptr;
	fwMesh* m_mesh = nullptr;

	void parseVertices(std::istringstream& infile, int nbVertices);
	void parseQuads(std::istringstream& infile, dfPalette *palette, int nbQuads);
	void addVertice(std::string& vertice, glm::vec3& color);
public:
	df3DO(dfFileSystem* fs, dfPalette* palette, std::string file);
	void add2scene(fwScene* scene, glm::vec3& position);
	~df3DO();
};