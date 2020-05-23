#include "df3DO.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "../dfFileSystem.h"
#include "../dfParseINF.h"
#include "../dfPalette.h"
#include "../../framework/fwGeometry.h"
#include "../../framework/fwMesh.h"
#include "../../framework/fwMaterial.h"
#include "../../framework/fwScene.h"

enum {
	x3DO_OBJECTS,
	x3DO_VERTICES,
	x3DO_POLYGONS,
	x3DO_PALETTE
};

static std::map<std::string, int> myTokens{
	{ "OBJECTS", x3DO_OBJECTS },
	{ "VERTICES", x3DO_OBJECTS },
	{ "POLYGONS", x3DO_POLYGONS},
	{ "PALETTE", x3DO_PALETTE }
};

static fwMaterial *materialFlat = nullptr;

df3DO::df3DO(dfFileSystem* fs, dfPalette* palette, std::string file) :
	dfModel(file)
{
	if (!materialFlat) {
		materialFlat = new fwMaterial("data/shaders/3do/3do_flat_vs.glsl", "data/shaders/3do/3do_flat_fs.glsl");
		materialFlat->addShader(FRAGMENT_SHADER, "data/shaders/3do/3do_flat_fs.glsl", DEFERED_RENDER);
	}
	char* sec = fs->load(DF_DARK_GOB, file);
	std::istringstream infile(sec);
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;

	while (std::getline(infile, line)) {
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		if (tokens[0] == "OBJECTS") {
			// m_objects.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "VERTICES") {
			if (tokens[1][0] == '0') {
				// VERTICES 00016 => total vertices
				// m_verticesIndex.resize(std::stoi(tokens[1]));
			}
			else {
				// read the number of vertices listed
				// VERTICES 16 => womm be followed by 16 vertices
				m_verticesIndex.resize(std::stoi(tokens[1]));
				parseVertices(infile, std::stoi(tokens[1]));
			}
		}
		else if (tokens[0] == "OBJECT") {
		}
		else if (tokens[0] == "QUADS") {
			parseQuads(infile, palette, std::stoi(tokens[1]));
		}
		else if (tokens[0] == "TRIANGLES") {
			parseTriangles(infile, palette, std::stoi(tokens[1]));
		}
	}

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", &m_vertices[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float), false);
	m_geometry->addAttribute("aColor", GL_ARRAY_BUFFER, &m_colors[0], 3, m_vertices.size() * sizeof(glm::vec2), sizeof(float), false);

	m_mesh = new fwMesh(m_geometry, materialFlat);
	m_mesh->rendering(m_shading);
	if (m_shading == fwMeshRendering::FW_MESH_POINT) {
		m_mesh->pointSize(16.0f);
	}
}

/**
 * Add the model directly into a scene
 */
void df3DO::add2scene(fwScene* scene, glm::vec3& position)
{
	fwMesh* mesh = m_mesh->clone();
	mesh->translate(position);
	mesh->set_scale(0.10);
	mesh->set_name(m_name);
	scene->addChild(mesh);
}

/**
 * Parse the vertice section
 */
void df3DO::parseVertices(std::istringstream & infile, int nbVertices)
{
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	char numVertice[255];
	int nbVertice = 0;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		// try to convert token 0 to a digit
		char* p;
		const char* numToken = tokens[0].c_str();
		strncpy_s(numVertice, numToken, sizeof(numVertice));
		numVertice[strlen(numVertice) - 1] = 0;	// remove the leading :
		float converted = strtof(numVertice, &p);

		if (p == numVertice) {
			// conversion failed because the input wasn't a number
			break;
		}
		else {
			m_verticesIndex[(int)converted] = glm::vec3(
				std::stof(tokens[1]),
				-std::stof(tokens[2]),
				std::stof(tokens[3])
			);
		}

		if ((int)converted == nbVertices - 1) {
			// just read the last vertice
			break;
		}
	}
}

/**
 * Parse a quad section
 */
void df3DO::parseQuads(std::istringstream& infile, dfPalette *palette, int nbQuads)
{
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	char numQuad[255];
	int nbIndex = 0;
	glm::ivec4 *rgba;
	glm::vec3 color;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		// try to convert token 0 to a digit
		char* p;
		const char* numToken = tokens[0].c_str();
		strncpy_s(numQuad, numToken, sizeof(numQuad));
		numQuad[strlen(numQuad) - 1] = 0;	// remove the leading :
		float converted = strtof(numQuad, &p);

		if (p == numQuad) {
			// conversion failed because the input wasn't a number
			break;
		}
		else {
			rgba = palette->getColor(std::stoi(tokens[5]), false);
			color.r = rgba->r / 255.0f;
			color.g = rgba->g / 255.0f;
			color.b = rgba->b / 255.0f;

			addVertice(tokens[1], color);
			addVertice(tokens[2], color);
			addVertice(tokens[3], color);

			addVertice(tokens[3], color);
			addVertice(tokens[4], color);
			addVertice(tokens[1], color);
		}

		if ((int)converted == nbQuads - 1) {
			// just read the last vertice
			break;
		}
	}
}

/**
 * Parse a triangle section
 */
void df3DO::parseTriangles(std::istringstream& infile, dfPalette* palette, int nbTriangles)
{
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	char numQuad[255];
	int nbIndex = 0;
	glm::ivec4* rgba;
	glm::vec3 color;

	while (std::getline(infile, line))
	{
		// ignore comment
		if (line.length() == 0) {
			continue;
		}

		// per token
		std::vector <std::string> tokens = dfParseTokens(line, tokenMap);
		if (tokens.size() == 0) {
			continue;
		}

		// try to convert token 0 to a digit
		char* p;
		const char* numToken = tokens[0].c_str();
		strncpy_s(numQuad, numToken, sizeof(numQuad));
		numQuad[strlen(numQuad) - 1] = 0;	// remove the leading :
		float converted = strtof(numQuad, &p);

		if (p == numQuad) {
			// conversion failed because the input wasn't a number
			break;
		}
		else {
			rgba = palette->getColor(std::stoi(tokens[4]), false);
			color.r = rgba->r / 255.0f;
			color.g = rgba->g / 255.0f;
			color.b = rgba->b / 255.0f;

			addVertice(tokens[1], color);
			addVertice(tokens[3], color);
			addVertice(tokens[2], color);

			if (tokens[5] == "vertex") {
				m_shading = fwMeshRendering::FW_MESH_POINT;
			}
		}

		if ((int)converted == nbTriangles - 1) {
			// just read the last vertice
			break;
		}
	}
}

/**
 * Create a new vertice with attribute
 */
void df3DO::addVertice(std::string& vertice, glm::vec3& color)
{
	int i = std::stoi(vertice);
	m_vertices.push_back( m_verticesIndex[i] );
	m_colors.push_back(color);
}

df3DO::~df3DO()
{
	delete m_geometry;
	delete m_mesh;
}
