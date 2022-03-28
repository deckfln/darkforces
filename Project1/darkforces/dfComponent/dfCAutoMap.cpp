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

	std::map<uint32_t, std::map<uint32_t, int32_t>> portalRender;

	for (auto sector : sectorsID) {

		for (auto wall : sector->m_walls) {

			// check if the wall was rendered from a mirrored portal
			if (wall->m_adjoint >= 0 && sector->layer() == wall->m_pAdjoint->layer()) {
				if (portalRender[wall->m_adjoint][wall->m_mirror] > 0) {
					wall->automap(portalRender[wall->m_adjoint][wall->m_mirror]);
					continue;
				}
			}

			// add new portals
			portalRender[sector->m_id][wall->m_index] = m_walls.size();
			wall->automap(portalRender[sector->m_id][wall->m_index]);

			// register the wall index
			m_wallsIndex[wall->id()] = m_walls.size();

			const glm::vec2& left = sector->m_vertices[wall->m_left];
			const glm::vec2& right = sector->m_vertices[wall->m_right];

			int32_t  colorIndex = 1 | (1 << 8);	// default color

			if (sector->flag() & dfSectorFlag::ELEVATOR) {
				colorIndex = 3 | (1 << 8);
			}
			else if (wall->m_adjoint >= 0) {

				if (wall->m_pAdjoint->flag() & dfSectorFlag::ELEVATOR) {
					colorIndex = 3 | (1 << 8);
				}
				else {
					colorIndex = 2 | (1 << 8);
				}
			}

			// record the layer in the wall information
			colorIndex |= (sector->layer() << 16);

			m_vertices.push_back(left);
			m_vertices.push_back(right);

			m_walls.push_back(colorIndex);
			m_walls.push_back(colorIndex);
		}

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
	m_uniRatio.set("ratio", &m_ratio);
	m_uniLayer.set("playerLayer", &m_layer);

	m_material->addUniform(&m_uniPosition);
	m_material->addUniform(&m_uniRatio);
	m_material->addUniform(&m_uniLayer);

	std::string vs = m_material->load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
	std::string fs = m_material->load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
	std::string gs = m_material->load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

	m_program = new glProgram(vs, fs, gs, "");

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos",
		&m_vertices[0],
		2,																// 2 float per point
		sizeof(glm::vec2) * m_vertices.size(),
		sizeof(float),
		false);

	m_geometry->addAttribute("aWall",
		GL_ARRAY_BUFFER,
		&m_walls[0],
		1,																// 2 float per point
		sizeof(int32_t) * m_walls.size(),
		sizeof(int32_t),
		false);

	m_vertexArray = new glVertexArray();
	m_geometry->enable_attributes(m_program);
	m_vertexArray->unbind();
}

//-------------------------------------------------------

/**
 * display a sector when the player enters
 */
void DarkForces::Component::AutoMap::onEnterSector(gaMessage* message)
{
	dfSector* sector = dynamic_cast<dfSector*>(message->m_pServer);

	m_layer = sector->layer();

	uint32_t index;
	for (auto wall : sector->m_walls) {
		index = wall->automap();
		m_walls[index] &= ~256;
		m_walls[index + 1] &= ~256;

		m_geometry->dirty("aWall");
	}
}

/**
 * set the new screen ratio
 */
void DarkForces::Component::AutoMap::onScreenResize(gaMessage* message)
{
	m_ratio = message->m_fvalue;
}

/**
 * hide/display the automap
 */
void DarkForces::Component::AutoMap::onShowAutomap(gaMessage* message)
{
	m_visible = !m_visible;
}

//-------------------------------------------------------

/**
 * create empty
 */
DarkForces::Component::AutoMap::AutoMap(void):
	gaComponent(DF_COMPONENT_AUTOMAP),
	fwHUDelement("DarkForces:automap")
{
	m_visible = false;	// hidden at start
}

/**
 * create
 */
DarkForces::Component::AutoMap::AutoMap(dfLevel* level) :
	gaComponent(DF_COMPONENT_AUTOMAP),
	fwHUDelement("DarkForces:automap")
{
	set(level);
	m_visible = false;	// hidden at start
}

/**
 * dedicated draw function
 */
void DarkForces::Component::AutoMap::draw(fwFlatPanel* panel)
{
	m_playerPosition.x = m_entity->position().x;
	m_playerPosition.y = m_entity->position().z;

	if (m_visible) {
		m_program->run();

		m_material->set_uniforms(m_program);
		m_geometry->draw(GL_LINES, m_vertexArray);
	}
}

/**
 *
 */
void DarkForces::Component::AutoMap::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::EVENT:
		switch (message->m_value) {
		case DarkForces::MessageEvent::ENTER_SECTOR:
			onEnterSector(message);
			break;
		}
		break;

	case gaMessage::Action::SCREEN_RESIZE:
		onScreenResize(message);
		break;

	case DarkForces::Message::AUTOMAP:
		onShowAutomap(message);
		break;
	}
}
