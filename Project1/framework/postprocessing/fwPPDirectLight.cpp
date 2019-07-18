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
static glProgram *Light_program = nullptr;

fwPPDirectLight::fwPPDirectLight(void)
{
}

glColorMap *fwPPDirectLight::draw(glGBuffer *colorMap, std::list <fwDirectionLight *>lights)
{
	if (Light_program == nullptr) {
		fwDirectionLight* first = lights.front();
		std::string define = "#define " + lights.front()->getDefine() + " " + std::to_string(lights.size()) + "\n";
		std::string vertex = Light.get_shader(VERTEX_SHADER);
		std::string fragment = Light.get_shader(FRAGMENT_SHADER);
		Light_program = new glProgram(vertex, fragment, "", define);

		geometry = new fwGeometry();
		quad = new glVertexArray();

		geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		geometry->addAttribute("aTexCoord", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
		geometry->enable_attributes(Light_program);
		quad->unbind();

		glm::vec2 size = colorMap->size();
		m_colorMap = new glColorMap(size.x, size.y, 1);
	}

	// copy into bloom buffer and expand the objects
	Light_program->run();

	// setup the GBuffer source
	Light.setSourceTexture(colorMap->getColorTexture(0), 
		colorMap->getColorTexture(1), 
		colorMap->getColorTexture(2),
		colorMap->getColorTexture(3)
		);

	// setup lights
	int i = 0;
	for (auto light : lights) {
		light->set_uniform(Light_program, i);
		i++;
	}

	Light.set_uniforms(Light_program);

	glTexture::resetTextureUnit();

	m_colorMap->bind();
	m_colorMap->clear();
	geometry->draw(GL_TRIANGLES, quad);

	return m_colorMap;
}

fwPPDirectLight::~fwPPDirectLight()
{
}
