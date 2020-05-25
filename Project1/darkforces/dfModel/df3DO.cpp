#include "df3DO.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

#include "../../framework/fwGeometry.h"
#include "../../framework/fwMesh.h"
#include "../../framework/fwMaterial.h"
#include "../../framework/fwScene.h"

#include "../dfFileSystem.h"
#include "../dfParseINF.h"
#include "../dfPalette.h"
#include "../dfBitmap.h"
#include "../dfAtlasTexture.h"

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

static std::map<std::string, int> myFill{
	{ "GOURAUD", 0 },
	{ "FLAT", 1 },
	{ "TEXTURE", 2 }
};

/**
 * Material to display 3DO
 */
static fwMaterial *materialFlat = nullptr;
static fwMaterial* materialTextured = nullptr;
static fwUniform* g_texture = nullptr;

/**
 *
 */
struct df3DOVertexIndex {
	glm::vec3 vertex;
	glm::vec3 texture;
};
struct df3DOQuadIndex {
	int vertexIndex[4];
	int textureIndex[4];
	glm::vec3 color;
	int fill;
};
static std::vector<struct df3DOVertexIndex> verticesIndex;		// Vertices 0:, 1:
static std::vector<struct df3DOQuadIndex> quadIndex;		// Vertices 0:, 1:
static std::vector<struct df3DOQuadIndex> triangleIndex;		// Vertices 0:, 1:
static int currentTexture;

/**
 *
 */
df3DO::df3DO(dfFileSystem* fs, dfPalette* palette, std::string file) :
	dfModel(file)
{
	char* sec = fs->load(DF_DARK_GOB, file);
	std::istringstream infile(sec);
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	int nbTexture = 0;
	bool withTexture = false;

	if (!materialFlat) {
		// material for flat objects
		materialFlat = new fwMaterial("data/shaders/3do/3do_flat_vs.glsl", "data/shaders/3do/3do_flat_fs.glsl");
		materialFlat->addShader(FRAGMENT_SHADER, "data/shaders/3do/3do_flat_fs.glsl", DEFERED_RENDER);

		materialTextured = new fwMaterial("data/shaders/3do/3do_texture_vs.glsl", "data/shaders/3do/3do_texture_fs.glsl");
		materialTextured->addShader(FRAGMENT_SHADER, "data/shaders/3do/3do_texture_fs.glsl", DEFERED_RENDER);
	}

	verticesIndex.resize(0);	// reset the local caches
	quadIndex.resize(0);
	triangleIndex.resize(0);
	currentTexture = -1;

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
				verticesIndex.resize(std::stoi(tokens[1]));
				parseVertices(infile, std::stoi(tokens[1]));
			}
		}
		else if (tokens[0] == "OBJECT") {
			if (verticesIndex.size() > 0) {
				buildObject();
			}
			verticesIndex.resize(0);	// reset the local caches
			quadIndex.resize(0);
			triangleIndex.resize(0);
			currentTexture = -1;
		}
		else if (tokens[0] == "TEXTURE") {
			if (tokens[1] == "VERTICES") {
				parseTextures(infile, std::stoi(tokens[2]));
				withTexture = true;
			}
			else if (tokens[1] == "QUADS") {
				parseTexturesQuad(infile, std::stoi(tokens[2]));
			}
			else if (tokens[1] == "TRIANGLES") {
				parseTexturesQuad(infile, std::stoi(tokens[2]));
			}
			else {
				currentTexture = std::stoi(tokens[1]);
			}
		}
		else if (tokens[0] == "TEXTURES") {
			m_textureNames.resize(std::stoi(tokens[1]));
		}
		else if (tokens[0] == "TEXTURE:") {
			m_textureNames[nbTexture++] = new dfBitmap(fs, tokens[1], palette);
		}
		else if (tokens[0] == "QUADS") {
			quadIndex.resize(std::stoi(tokens[1]));
			parseQuads(infile, palette, std::stoi(tokens[1]));
		}
		else if (tokens[0] == "TRIANGLES") {
			triangleIndex.resize(std::stoi(tokens[1]));
			parseTriangles(infile, palette, std::stoi(tokens[1]));
		}
	}

	// build the remaining object
	buildObject();

	// build the atlas texture
	if (m_textureNames.size() > 0) {
		std::vector<dfBitmapImage*> images;
		for (auto tex : m_textureNames) {
			images.push_back(tex->getImage());
		}
		m_atlasTexture = new dfAtlasTexture(images);
		m_atlasTexture->save("D:/dev/Project1/Project1/images/"+file+".png");
		m_atlasTexture->bindToMaterial(materialTextured, "texture");
	}

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", &m_vertices[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float), false);
	m_geometry->addAttribute("aColor", GL_ARRAY_BUFFER, &m_colors[0], 3, m_vertices.size() * sizeof(glm::vec3), sizeof(float), false);
	if (withTexture) {
		m_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, &m_textures[0], 3, m_textures.size() * sizeof(glm::vec3), sizeof(float), false);
	}


	if (m_textureNames.size() > 0) {
		m_mesh = new fwMesh(m_geometry, materialTextured);
	}
	else {
		m_mesh = new fwMesh(m_geometry, materialFlat);
	}
	m_mesh->rendering(m_shading);
	if (m_shading == fwMeshRendering::FW_MESH_POINT) {
		m_mesh->pointSize(12.0f);
	}
}

/**
 * Add the model directly into a scene
 */
fwMesh* df3DO::clone(void)
{
	return m_mesh->clone();
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
			verticesIndex[(int)converted].vertex = glm::vec3(
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

			quadIndex[(int)converted].vertexIndex[0] = std::stoi(tokens[1]);
			quadIndex[(int)converted].vertexIndex[1] = std::stoi(tokens[2]);
			quadIndex[(int)converted].vertexIndex[2] = std::stoi(tokens[3]);
			quadIndex[(int)converted].vertexIndex[3] = std::stoi(tokens[4]);
			quadIndex[(int)converted].color = color;
			quadIndex[(int)converted].fill = myFill[tokens[6]];
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
			if (tokens[5] == "vertex") {
				// From DEATH.3DO it seems the first triagnle color is then applied to all vertices
				if (m_vertices.size() == 0) {
					rgba = palette->getColor(std::stoi(tokens[4]), false);
					color.r = rgba->r / 255.0f;
					color.g = rgba->g / 255.0f;
					color.b = rgba->b / 255.0f;

					m_perVertexColor = color;
				}
				else {
					color = m_perVertexColor;
				}
			}
			else {
				rgba = palette->getColor(std::stoi(tokens[4]), false);
				color.r = rgba->r / 255.0f;
				color.g = rgba->g / 255.0f;
				color.b = rgba->b / 255.0f;
			}

			triangleIndex[(int)converted].vertexIndex[0] = std::stoi(tokens[1]);
			triangleIndex[(int)converted].vertexIndex[1] = std::stoi(tokens[2]);
			triangleIndex[(int)converted].vertexIndex[2] = std::stoi(tokens[3]);
			triangleIndex[(int)converted].color = color;
			triangleIndex[(int)converted].fill = myFill[tokens[5]];

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
 * Parse the texture mapping
 */
void df3DO::parseTextures(std::istringstream& infile, int nbVertives)
{
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	char numQuad[255];
	int nbIndex = 0;

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
			verticesIndex[(int)converted].texture = glm::vec3(
				std::stof(tokens[1]),
				std::stof(tokens[2]),
				float(currentTexture));
		}

		if ((int)converted == nbVertives - 1) {
			// just read the last vertice
			break;
		}
	}
}

void df3DO::parseTexturesQuad(std::istringstream& infile, int nbQuads)
{
	std::string line, dump;
	std::map<std::string, std::string> tokenMap;
	char numQuad[255];
	int nbIndex = 0;

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
	
		if ((int)converted == nbQuads - 1) {
			// just read the last vertice
			break;
		}
	}
}

/**
 * Create a new vertice with attribute
 */
void df3DO::addVertice(df3DOQuadIndex *quad, int vertexIDX)
{
	static glm::vec3 flat = glm::vec3(0, 0, -1);

	m_vertices.push_back( verticesIndex[ quad->vertexIndex[vertexIDX] ].vertex );
	m_colors.push_back(quad->color);

	if (quad->fill == 2) {
		m_textures.push_back(verticesIndex[quad->vertexIndex[vertexIDX]].texture);
	}
	else {
		m_textures.push_back(flat);
	}
}

/**
 * Build openGL object using the caches
 */
void df3DO::buildObject(void)
{
	// build the quads
	for (auto& quad : quadIndex) {
		addVertice(&quad, 0);
		addVertice(&quad, 1);
		addVertice(&quad, 2);

		addVertice(&quad, 2);
		addVertice(&quad, 3);
		addVertice(&quad, 0);
	}

	// build the triangles
	for (auto& quad : triangleIndex) {
		addVertice(&quad, 0);
		addVertice(&quad, 2);
		addVertice(&quad, 1);
	}
}

df3DO::~df3DO()
{
	delete m_geometry;
	delete m_mesh;

	for (auto tex : m_textureNames) {
		delete tex;
	}

	if (m_atlasTexture) {
		delete m_atlasTexture;
	}
}
