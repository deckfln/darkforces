#include "fwSprites.h"

#include "../glEngine/glUniformBuffer.h"

static fwMaterial* spriteMaterial = nullptr;

fwSprites::fwSprites(int size) :
	m_size(size)
{
	m_classID |= Framework::ClassID::SPRITES;
}

fwSprites::fwSprites(glm::vec3 *position, int nb, fwTexture *texture, float radius) :
	m_size(nb)
{
	m_classID |= Framework::ClassID::SPRITES;

	set(position, texture, radius);
}

void fwSprites::set(glm::vec3* position, fwTexture* texture, float radius)
{
	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", position, 3, sizeof(glm::vec3) * m_size, sizeof(float), false);	// do not delete on exit
	m_geometry->setBoundingsphere(radius);

	if (spriteMaterial == nullptr) {
		spriteMaterial = new fwMaterial("shaders/sprite/vertex.glsl", "shaders/sprite/fragment.glsl");
		spriteMaterial->addShader(FRAGMENT_SHADER, "shaders/sprite/fragment_defered.glsl", DEFERED_RENDER);
	}

	if (m_material == nullptr) {
		m_material = spriteMaterial;
	}

	m_material->addTexture("texture", texture);
}

void fwSprites::draw(glProgram *program)
{
	// create one VAO by shader class
	GLuint id = buildVAO(program);

	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	// todo: this trigger an alert in nsigh :GL_INVALID_ENUM
	//glEnable(GL_POINT_SMOOTH);

	m_geometry->draw(GL_POINTS, m_vao[id]);

	//glDisable(GL_POINT_SMOOTH);
	glDisable(GL_PROGRAM_POINT_SIZE);
}

fwSprites::~fwSprites()
{
	delete m_geometry;
}