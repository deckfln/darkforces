#include "fwSkyline.h"

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../../glEngine/glTexture.h"
#include "../fwUniform.h"
#include "../fwGeometry.h"

const std::string ring_vs = "framework/shaders/background/skyline_vs.glsl";
const std::string ring_fs = "framework/shaders/background/skyline_fs.glsl";

static std::vector<glm::vec3> gVertices;
static std::vector<glm::vec2> gTexture;
static std::vector<int> gIndex;

fwSkyline::fwSkyline(unsigned char* data, int width, int height, int channels, int repeat_x, int repeat_y) :
	fwBackground(ring_vs, ring_fs),
	m_repeat(glm::vec2(repeat_x * 2, repeat_y))
{
	// init the ring
	if (gVertices.size() == 0) {
		for (float a = 0; a < 2 * M_PI; a += M_PI/8.0) {
			gVertices.push_back(glm::vec3(cos(a), 0.0, sin(a)));
			gVertices.push_back(glm::vec3(cos(a), 2.0, sin(a)));

			gTexture.push_back(glm::vec2(0, a / (2 * M_PI)));
			gTexture.push_back(glm::vec2(1, a / (2 * M_PI)));
		}

		for (auto v = 0; v < gVertices.size() - 2; v +=2 ) {
			gIndex.push_back(v);
			gIndex.push_back(v + 2);
			gIndex.push_back(v + 1);

			gIndex.push_back(v + 1);
			gIndex.push_back(v + 2);
			gIndex.push_back(v + 3);
		}
		gVertices.push_back(glm::vec3(cos(2*M_PI), 0.0, sin(2 * M_PI)));
		gVertices.push_back(glm::vec3(cos(2 * M_PI), 2.0, sin(2 * M_PI)));

		gTexture.push_back(glm::vec2(0, 1));
		gTexture.push_back(glm::vec2(1, 1));

		int v = gVertices.size() - 4;
		gIndex.push_back(v);
		gIndex.push_back(v + 2);
		gIndex.push_back(v + 1);

		gIndex.push_back(v + 1);
		gIndex.push_back(v + 2);
		gIndex.push_back(v + 3);

		m_geometry->addVertices("aPos", &gVertices[0], 3, gVertices.size() * sizeof(glm::vec3), sizeof(float), false);
		m_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, &gTexture[0], 3, gTexture.size() * sizeof(glm::vec2), sizeof(float), false);
		m_geometry->addIndex(&gIndex[0], 1, gIndex.size() * sizeof(int), sizeof(int), false);

		m_geometry->enable_attributes(m_program);
		m_cube->unbind();
	}

	m_texture = new glTexture(data, width, height, channels, GL_FILTER);

	m_uniforms.push_back(new fwUniform("skyline", m_texture));
	m_uniforms.push_back(new fwUniform("repeat", &m_repeat));
}

fwSkyline::~fwSkyline()
{
}