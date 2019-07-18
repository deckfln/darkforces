#include "fwPPDirectLight.h"

#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "../../glad/glad.h"
#include "../materials/fwMaterialDirectionalLight.h"

static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f, -1.0f,

	-1.0f,  1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f
};

static float quadUvs[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

static fwMaterialDirectionalLight Light;
enum {NO_SHADOW, SHADOW};
static glProgram* Light_program[2] = { nullptr, nullptr };
static bool init = false;

fwPPDirectLight::fwPPDirectLight(void)
{
}

void fwPPDirectLight::drawLight(std::list <fwDirectionLight*>lights, glGBuffer* colorMap, glProgram *program, glVertexArray *quad)
{
	program->run();

	// setup the GBuffer source
	Light.setSourceTexture(colorMap->getColorTexture(0),
		colorMap->getColorTexture(1),
		colorMap->getColorTexture(2),
		colorMap->getColorTexture(3)
	);

	// setup lights
	int i = 0;
	for (auto light : lights) {
		light->set_uniform(program, i);
		i++;
	}

	Light.set_uniforms(program);

	glTexture::resetTextureUnit();

	m_colorMap->bind();
	m_colorMap->clear();
	geometry->draw(GL_TRIANGLES, quad);
}

glColorMap *fwPPDirectLight::draw(glGBuffer *colorMap, std::list <fwDirectionLight *>lights)
{
	if (!init) {
		init = true;

		// no shadow program
		fwDirectionLight* first = lights.front();
		std::string define = "#define " + lights.front()->getDefine() + " " + std::to_string(lights.size()) + "\n";
		std::string vertex = Light.get_shader(VERTEX_SHADER);
		std::string fragment = Light.get_shader(FRAGMENT_SHADER);

		Light_program[NO_SHADOW] = new glProgram(vertex, fragment, "", define);

		// with shadow program
		define += "#define SHADOWMAP\n";
		Light_program[SHADOW] = new glProgram(vertex, fragment, "", define);

		geometry = new fwGeometry();

		for (int i = 0; i < 2; i++) {
			quad[i] = new glVertexArray();
			geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
			geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
			geometry->enable_attributes(Light_program[i]);
			quad[i]->unbind();
		}

		glm::vec2 size = colorMap->size();
		m_colorMap = new glColorMap(size.x, size.y, 1);

	}

	// no shadow lights
	int shd = NO_SHADOW;
	if (((fwObject3D*)lights.front())->castShadow()) {
		shd = SHADOW;
	}
	drawLight(lights, colorMap, Light_program[shd], quad[shd]);

	return m_colorMap;
}

fwPPDirectLight::~fwPPDirectLight()
{
}
