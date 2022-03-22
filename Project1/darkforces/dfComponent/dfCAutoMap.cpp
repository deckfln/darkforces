#include "dfCAutoMap.h"

#include <map>

#include "../../glEngine/glProgram.h"
#include "../../glEngine/glTextureArray.h"

#include "../../framework/fwGeometry.h"
#include "../../framework/fwMaterial.h"

#include "../../gaEngine/World.h"

#include "../dfComponent.h"
#include "../dfLevel.h"

/**
 *
 */
void DarkForces::Component::AutoMap::set(dfLevel* level)
{
	m_level = level;

	// parse all sectors and sort them by layer
	std::vector<dfSector*>& sectorsID = m_level->sectorsID();

	std::map<uint32_t, std::vector<dfSector*>> layers;

	for (auto sector : sectorsID) {
		layers[sector->layer()].push_back(sector);
	}

	// allocate a 2D buffer to hold walls for each layer
	for (auto& layer : layers) {
		uint32_t nbWalls = 0;
		for (auto sector : layer.second) {
			nbWalls += sector->m_walls.size();
		}
		m_verticesPerLayer[layer.first].m_vertices.resize(nbWalls * 2);	// maximum vertices that can be stored
		m_verticesPerLayer[layer.first].m_nbVertices = 0;				// start empty
	}

	// allocate a 2D buffer to hold walls for each layer
	for (auto& layer : layers) {
		uint32_t nbVertices = 0;
		for (auto sector : layer.second) {
			for (auto wall : sector->m_walls) {
				m_verticesPerLayer[layer.first].m_vertices[nbVertices] = sector->m_vertices[wall->m_left] / 10.0f;
				m_verticesPerLayer[layer.first].m_vertices[nbVertices + 1] = sector->m_vertices[wall->m_right] / 10.0f;

				nbVertices += 2;
			}
		}
		m_verticesPerLayer[layer.first].m_nbVertices = nbVertices;				// start empty
	}

	static std::map<ShaderType, std::string> g_subShaders = {
		{VERTEX_SHADER, "darkforces/shaders/hud/automap_vs.glsl"},
		{FRAGMENT_SHADER, "darkforces/shaders/hud/automap_fs.glsl"}
	};

	static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// m_positions   // texCoords
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f, -1.0f,

		-1.0f,  1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f
	};

	m_material = new fwMaterial(g_subShaders);
	m_uniPosition.set("player", &m_playerPosition);
	m_material->addUniform(&m_uniPosition);

	std::string vs = m_material->load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
	std::string fs = m_material->load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
	std::string gs = m_material->load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

	m_program = new glProgram(vs, fs, gs, "");

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos",
		&m_verticesPerLayer[1].m_vertices[0],
		2,																// 2 float per point
		sizeof(float) * 2 * m_verticesPerLayer[1].m_vertices.size(),
		ARRAY_SIZE_OF_ELEMENT(m_verticesPerLayer[1].m_vertices),
		false);

	m_vertexArray = new glVertexArray();
	m_geometry->enable_attributes(m_program);
	m_vertexArray->unbind();
}


/**
 * create empty
 */
DarkForces::Component::AutoMap::AutoMap(void):
	gaComponent(DF_COMPONENT_AUTOMAP),
	fwHUDelement("DarkForces:automap")
{
}

/**
 * create
 */
DarkForces::Component::AutoMap::AutoMap(dfLevel* level) :
	gaComponent(DF_COMPONENT_AUTOMAP),
	fwHUDelement("DarkForces:automap")
{
	set(level);
}

/**
 * dedicated draw function
 */
void DarkForces::Component::AutoMap::draw(fwFlatPanel* panel)
{
	m_program->run();
	m_playerPosition.x = m_entity->position().x;
	m_playerPosition.y = m_entity->position().z;

	m_material->set_uniforms(m_program);
	m_geometry->draw(GL_LINES, m_vertexArray);
}
